# E-Commerce Management System

## Deskripsi Proyek
E-Commerce Management System adalah aplikasi berbasis CLI (Command Line Interface) yang dirancang untuk mengelola transaksi antara pengguna dan penjual. Sistem ini mencakup fitur-fitur seperti checkout, pengembalian barang, pelacakan transaksi, serta pengelolaan saldo pengguna.

## Fitur Utama
1. **Checkout Barang**
   - Pengguna dapat memilih barang di keranjang dan menyelesaikan transaksi.
   - Mendukung berbagai pilihan jasa pengiriman.
   - Estimasi waktu pengiriman otomatis dihitung berdasarkan jarak tujuan.

2. **Pengembalian Barang (Return)**
   - Barang dapat dikembalikan jika telah diterima dalam waktu kurang dari 7 hari.
   - Saldo pengguna akan dikembalikan secara otomatis jika pengembalian disetujui.

3. **Pelacakan Transaksi**
   - Pengguna dapat melihat riwayat semua transaksi mereka.
   - Informasi termasuk waktu transaksi, status, dan estimasi pengiriman.

4. **Pengelolaan Saldo**
   - Saldo pengguna diperbarui secara otomatis setelah transaksi berhasil atau pengembalian disetujui.

5. **Sistem Penjual (Seller)**
   - Penjual dapat memproses pengembalian barang dan menyetujui pengembalian dana.

## Struktur File
```
project-directory/
├── database/
│   ├── cart.txt
│   ├── city.txt
│   ├── products.txt
│   ├── transactions.txt
│   └── users.txt
├── src/
│   ├── main.c
│   ├── checkout.c
│   ├── return.c
│   ├── transaction.c
│   └── utils.c
└── README.md
```

## Cara Menggunakan
### Prasyarat
1. Compiler C (misalnya GCC).
2. File database yang sesuai di dalam folder `database/`.

### Menjalankan Program
1. Clone repository ini:
   ```bash
   git clone https://github.com/username/repo-name.git
   ```
2. Navigasi ke direktori proyek:
   ```bash
   cd repo-name
   ```
3. Kompilasi program:
   ```bash
   gcc -o ecommerce src/*.c
   ```
4. Jalankan aplikasi:
   ```bash
   ./ecommerce
   ```

## Format Data
### users.txt
```
ID|Nama|Username|Email|Alamat|Saldo
```
Contoh:
```
1|ihsan|ihsanrestuadi|ihsanlearn@gmail.com|solo|966
```

### cart.txt
```
Nama Pemilik|Item ID|Product ID|Qty|Harga|Nama Produk|Deskripsi Produk
```
Contoh:
```
ihsan|1|1|1|20000000|Laptop Gaming|Laptop dengan performa tinggi untuk gaming dan editing.
```

### transactions.txt
```
Transaction ID|User ID|Waktu|Status|Estimasi Pengiriman|Jasa Pengiriman
```
Contoh:
```
1|1|Sun Dec 15 22:06:47 2024|Menunggu|Thu Dec 20 2024|JNE
```

## Kontribusi
1. Fork repository ini.
2. Buat branch baru:
   ```bash
   git checkout -b feature-name
   ```
3. Lakukan perubahan dan commit:
   ```bash
   git commit -m "Deskripsi perubahan"
   ```
4. Push ke branch:
   ```bash
   git push origin feature-name
   ```
5. Buat pull request.

## Lisensi
Proyek ini dilisensikan di bawah [MIT License](LICENSE).

---

## Kontak
Jika ada pertanyaan, hubungi:
- **Nama**: Ihsan
- **Email**: ihsanlearn@gmail.com

