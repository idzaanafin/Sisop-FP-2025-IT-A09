# Final Project Sistem Operasi IT

## Peraturan
1. Waktu pengerjaan dimulai hari Kamis (19 Juni 2025) setelah soal dibagikan hingga hari Rabu (25 Juni 2025) pukul 23.59 WIB.
2. Praktikan diharapkan membuat laporan penjelasan dan penyelesaian soal dalam bentuk Readme(github).
3. Format nama repository github “Sisop-FP-2025-IT-[Kelas][Kelompok]” (contoh:Sisop-FP-2025-IT-A01).
4. Setelah pengerjaan selesai, seluruh source code dan semua script bash, awk, dan file yang berisi cron job ditaruh di github masing - masing kelompok, dan link github dikumpulkan pada form yang disediakan. Pastikan github di setting ke publik.
5. Commit terakhir maksimal 10 menit setelah waktu pengerjaan berakhir. Jika melewati maka akan dinilai berdasarkan commit terakhir.
6. Jika tidak ada pengumuman perubahan soal oleh asisten, maka soal dianggap dapat diselesaikan.
7. Jika ditemukan soal yang tidak dapat diselesaikan, harap menuliskannya pada Readme beserta permasalahan yang ditemukan.
8. Praktikan tidak diperbolehkan menanyakan jawaban dari soal yang diberikan kepada asisten maupun praktikan dari kelompok lainnya.
9. Jika ditemukan indikasi kecurangan dalam bentuk apapun di pengerjaan soal final project, maka nilai dianggap 0.
10. Pengerjaan soal final project sesuai dengan modul yang telah diajarkan.

## Kelompok 09

Nama | NRP
--- | ---
Ahmad Idza Anafin | 5027241017
Rizqi Akbar Sukirman Putra | 5027241044
Dina Rahmadani | 5027241065
Mochammad Atha Tajuddin | 5027241093
Naufal Ardhana | 5027241118

## Deskripsi Soal
Read Only File 
Buatlah sebuah FUSE file system sederhana yang me-mount sebuah direktori dalam mode read-only. Kalian hanya dapat melihat dan membaca file, serta masuk ke dalam subdirektori. Maka dari itu, setiap upaya untuk memodifikasi (membuat, menulis, menghapus) harus gagal dengan error "Gabisa ya huhu".

### Catatan

Struktur repository:
```
Sisop-FP-2025-IT-A09
├── src
│ └── readonly_fs.c 
└─── README.md 
```

## Pengerjaan

### 1. Implemenetasi FUSE  

**Teori**

FUSE (*Filesystem in Userspace*) adalah framework untuk mengembangkan sistem file di ruang pengguna tanpa perlu menulis modul kernel. FUSE terdiri dari dua komponen utama:
- Modul kernel: menerima request dari sistem dan meneruskannya ke daemon pengguna.
- FUSE daemon: menangani operasi file system secara logika.

Salah satu keuntungan utama dari FUSE adalah kemudahan pengembangan dan keamanan karena crash di user-space tidak menyebabkan sistem crash.

Dalam konteks ini, sistem file dibuat hanya untuk **read-only** itu berarti jika dalam file permission maka level hak aksesnya ialah **004** karena sistem file dibuat dengan hak akses non-root serta read-only (dengan urutan owner,groups,public) , sehingga operasi seperti write, create, unlink, mkdir, rename harus diblokir. Error khas untuk filesystem semacam ini adalah EROFS (*Error Read-Only File System*).

**Solusi**

Solusi diimplementasikan dalam file `src/readonly_fs.c`. Beberapa poin penting:

1. **Mount Source Folder**  
   Direktori sumber ditentukan sebagai:
   ```c
   static const char *source_dir = "/Users/dina-r/Desktop/finalssp/source_dir"; (direktori menyesuaikan yang ingin dimount)
2. **Operasi yang Diizinkan**
   
   Fungsi berikut diimplementasikan untuk mendukung operasi navigasi dan baca:
   `getarr`
   `readdir`
   `open`
   `read`
4. **Operasi yang Ditolak**
   
   Semua fungsi modifikasi dialihkan ke satu fungsi `xmp_writeblock()` yang menampilkan pesan dan mengembalikan `-EROFS`:
   ```
   static int xmp_writeblock() {
    fprintf(stderr, "Gabisa ya huhu\n");
    return -EROFS;
   ```
   Kemudian fungsi tersebut dipasang pada:
   ```
   .mkdir   = (void*) xmp_writeblock,
   .unlink  = (void*) xmp_writeblock,
   .rmdir   = (void*) xmp_writeblock,
   .rename  = (void*) xmp_writeblock,
   .write   = (void*) xmp_writeblock,
   .create  = (void*) xmp_writeblock,
   ```
5. **Kompilasi dan Eksekusi**
   Kompilasi memerlukan librari fuse tambahan
   ## Linux
      - gcc -Wall namefile.c -o namefile `pkg-config fuse3 --cflags --libs`
   ## Mac
      - gcc -Wall -lfuse namefile.c -o namefile

...

**Video Menjalankan Program**
...

## Daftar Pustaka

1. Vangoor, B. K. R., Tarasov, V., & Zadok, E. (2017). *To FUSE or Not to FUSE: Performance of User‑Space File Systems*. FAST ’17: 15th USENIX Conference on File and Storage Technologies.
   https://www.usenix.org/conference/fast17/technical-sessions/presentation/vangoor
2. Vangoor, B. K. R., Agarwal, P., Mathew, M., et al. (2019). *Performance and Resource Utilization of FUSE User‑Space File Systems*. ACM Transactions on Storage, 15(2), Article 15. DOI: 10.1145/3310148
3. Maas, A. (2019). *Writing a less simple yet stupid filesystem using FUSE in C*. maastaar.net.
 https://maastaar.net/fuse/linux/filesystem/c/2019/09/28/writing-less-simple-yet-stupid-filesystem-using-FUSE-in-C/
   

