# pgQR: Generate QR Codes with Postgres

<p align="center"> <img src="assets/pgqr.png" alt="QR Code"> </p>

If you were at the latest [PG Conf EU 2024](https://2024.pgconf.eu),
you probably noticed that QR codes were extremely popular everywhere!

So naturally, here's an extension that brings QR support to Postgres!

## Usage

Encode `text` and store the QR code in a file.

```tsql
SELECT qr('No more Greek food!', '/tmp/message.png');
```

You can also generate a QR as ASCII

```tsql                                                     
SELECT qr_ascii('Hello World!");
```

## Installation

```
cd /tmp
git clone https://github.com/Florents-Tselai/pgQR.git
cd pgQR
make
make install # may need sudo
```

```tsql
CREATE EXTENSION qr;
```

> [!WARNING]
> Don't use this for encryption!
