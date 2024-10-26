#include "postgres.h"

#include "fmgr.h"
#include "qrcodegen.h"
#include "utils/builtins.h"
#include "png.h"
#if PG_VERSION_NUM >= 160000
#include "varatt.h"
#endif

#if PG_VERSION_NUM < 130000
#define TYPALIGN_DOUBLE 'd'
#define TYPALIGN_INT 'i'
#endif


PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(qr_encode_ascii);

Datum
qr_encode_ascii(PG_FUNCTION_ARGS) {
    const char *txt = text_to_cstring(PG_GETARG_TEXT_P(0));

    int size, border, line_len, total_len, pos;
    char *result;
    uint8_t qrcode[qrcodegen_BUFFER_LEN_MAX];
    uint8_t tempBuffer[qrcodegen_BUFFER_LEN_MAX];

    bool ok = qrcodegen_encodeText(txt, tempBuffer, qrcode,
                                   qrcodegen_Ecc_HIGH, qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX,
                                   qrcodegen_Mask_AUTO, true);


    size = qrcodegen_getSize(qrcode);
    border = 4;

    // Calculate the buffer size: each pixel takes 2 chars, plus newline characters
    line_len = (size + 2 * border) * 2 + 1; // Each line: (2 * border + size) * 2 chars + 1 newline
    total_len = line_len * (size + 2 * border) + 1; // Total buffer size

    result = palloc0(total_len);


    // Collect QR code to result buffer
    pos = 0;
    for (int y = -border; y < size + border; y++) {
        for (int x = -border; x < size + border; x++) {
            pos += snprintf(result + pos, total_len - pos, "%s", qrcodegen_getModule(qrcode, x, y) ? "##" : "  ");
        }
        pos += snprintf(result + pos, total_len - pos, "\n");
    }

    PG_RETURN_TEXT_P(cstring_to_text(result));
}

PG_FUNCTION_INFO_V1(qr_encode_png);


static void saveQrToPng(const uint8_t qrcode[], const char *path) {
    int size = qrcodegen_getSize(qrcode);
    int border = 4;
    int scale = 10;  // Each QR module will be a 10x10 pixel block
    int imageSize = (size + 2 * border) * scale;

    FILE *file = fopen(path, "wb");
    if (!file) {
        fprintf(stderr, "Error opening file %s\n", path);
        exit(1);
    }

    // Initialize libpng structures
    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        fclose(file);
        fprintf(stderr, "Failed to create PNG write structure.\n");
        exit(1);
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        png_destroy_write_struct(&png, (png_infopp)NULL);
        fclose(file);
        fprintf(stderr, "Failed to create PNG info structure.\n");
        exit(1);
    }

    if (setjmp(png_jmpbuf(png))) {
        png_destroy_write_struct(&png, &info);
        fclose(file);
        fprintf(stderr, "Error during PNG creation.\n");
        exit(1);
    }

    png_init_io(png, file);

    // Write header (8 bit greyscale)
    png_set_IHDR(png, info, imageSize, imageSize, 8, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);

    // Allocate memory for one row (1 byte per pixel)
    png_bytep row = (png_bytep)malloc(imageSize * sizeof(png_byte));

    // Write image data
    for (int y = -border; y < size + border; y++) {
        for (int sy = 0; sy < scale; sy++) {  // Scale each row
            for (int x = -border; x < size + border; x++) {
                bool module = (x >= 0 && y >= 0 && x < size && y < size) && qrcodegen_getModule(qrcode, x, y);
                unsigned char color = module ? 0 : 255;  // Black (0) or white (255)

                // Fill the row for the scaled QR module
                for (int sx = 0; sx < scale; sx++) {
                    row[(x + border) * scale + sx] = color;
                }
            }
            png_write_row(png, row);  // Write each scaled row
        }
    }

    // End write
    png_write_end(png, NULL);

    // Cleanup
    fclose(file);
    png_destroy_write_struct(&png, &info);
    free(row);

    printf("PNG image saved to %s\n", path);
}


Datum qr_encode_png(PG_FUNCTION_ARGS) {
    const char *txt = text_to_cstring(PG_GETARG_TEXT_P(0));
    const char *file = text_to_cstring(PG_GETARG_TEXT_P(1));

    uint8_t qrcode[qrcodegen_BUFFER_LEN_MAX];
    uint8_t tempBuffer[qrcodegen_BUFFER_LEN_MAX];

    bool ok = qrcodegen_encodeText(txt, tempBuffer, qrcode,
                                   qrcodegen_Ecc_HIGH, qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX,
                                   qrcodegen_Mask_AUTO, true);

    if (!ok)
        PG_RETURN_BOOL(false);

    saveQrToPng(qrcode, file);
    PG_RETURN_BOOL(true);
}