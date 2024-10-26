# pgQR: Generate QR Codes with Postgres

<p align="center"> <img src="assets/pgqr.png" alt="QR Code"> </p>

If you were at the latest [PG Conf EU 2024](https://2024.pgconf.eu),
you probably noticed that QR codes were extremely popular everywhere!

So naturally, here's an extension that brings QR support to Postgres!

```tsql
select qr('No more Greek food!', '/tmp/message.png')
```

## Installation

```
cd /tmp
git clone https://github.com/Florents-Tselai/pgQR.git
cd pgqr
make
make install # may need sudo
```

**NOTE**
Don't use this for encryption!
