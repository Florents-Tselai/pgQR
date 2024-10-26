CREATE FUNCTION qr_ascii(text) RETURNS text
AS
'MODULE_PATHNAME',
'qr_encode_ascii' LANGUAGE C;

CREATE FUNCTION qr(text, text) RETURNS bool
AS
'MODULE_PATHNAME',
'qr_encode_png'
    LANGUAGE C;

