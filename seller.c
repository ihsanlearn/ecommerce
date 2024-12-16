#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "utils.h"
#include "seller.h"

int getNextProductId() {
    FILE *file = fopen(PRODUCTS_FILE, "r");
    if (!file) {
        printf("Error: Tidak dapat membuka file %s\n", PRODUCTS_FILE);
        return 1;
    }

    int maxId = 0, id;
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file)) {
        sscanf(buffer, "%d|%*s", &id);
        if (id > maxId) {
            maxId = id;
        }
    }

    fclose(file);
    return maxId + 1;
}

void addProducts() {
    clearScreen();
    printf("--- Tambahkan Produk Baru ---\n");

    int id = getNextProductId();
    Product *newProduct = malloc(sizeof(Product));

    if (!newProduct) {
        printf("Error: Tidak dapat mengalokasikan memori untuk produk baru.\n");
        return;
    }

    printf("Nama Produk: ");
    fgets(newProduct->name, sizeof(newProduct->name), stdin);
    removeNewline(newProduct->name);

    printf("Kategori: ");
    fgets(newProduct->category, sizeof(newProduct->category), stdin);
    removeNewline(newProduct->category);

    printf("Harga: ");
    scanf("%d", &newProduct->price);

    printf("Berat (gram): ");
    scanf("%d", &newProduct->weight);

    printf("Stok: ");
    scanf("%d", &newProduct->stock);

    while (getchar() != '\n');

    printf("Deskripsi: ");
    fgets(newProduct->description, sizeof(newProduct->description), stdin);
    removeNewline(newProduct->description);

    newProduct->id = id;
    newProduct->sold = 0;

    FILE *file = fopen(PRODUCTS_FILE, "a");
    if (!file) {
        printf("Error: Tidak dapat membuka file %s untuk menulis.\n", PRODUCTS_FILE);
        free(newProduct);
        return;
    }

    fprintf(file, "%d|%s|%s|%d|%d|%d|%d|%s\n", newProduct->id, newProduct->name, newProduct->category, newProduct->price, newProduct->weight, newProduct->stock, newProduct->sold, newProduct->description);
    fclose(file);

    printf("Produk berhasil ditambahkan!\n");
    free(newProduct);
}

void updateProductStock() {
    clearScreen();
    printf("--- Update Stok Produk ---\n");

    int productId, newStock;
    printf("Masukkan ID Produk: ");
    scanf("%d", &productId);

    printf("Masukkan jumlah stok baru: ");
    scanf("%d", &newStock);

    FILE *file = fopen(PRODUCTS_FILE, "r");
    FILE *tempFile = fopen("database/temp_products.txt", "w");

    if (!file || !tempFile) {
        printf("Error: Tidak dapat membuka file produk.\n");
        if (file) fclose(file);
        if (tempFile) fclose(tempFile);
        return;
    }

    char buffer[256];
    int found = 0;
    Product currentProduct;

    while (fgets(buffer, sizeof(buffer), file)) {
        sscanf(buffer, "%d|%[^|]|%[^|]|%d|%d|%d|%d|%[^\n]", &currentProduct.id, currentProduct.name, currentProduct.category, &currentProduct.price, &currentProduct.weight, &currentProduct.stock, &currentProduct.sold, currentProduct.description);
        if (currentProduct.id == productId) {
            found = 1;
            currentProduct.stock = newStock;
        }
        fprintf(tempFile, "%d|%s|%s|%d|%d|%d|%d|%s\n", currentProduct.id, currentProduct.name, currentProduct.category, currentProduct.price, currentProduct.weight, currentProduct.stock, currentProduct.sold, currentProduct.description);
    }

    fclose(file);
    fclose(tempFile);

    if (found) {
        remove(PRODUCTS_FILE);
        rename("database/temp_products.txt", PRODUCTS_FILE);
        printf("Stok produk berhasil diperbarui!\n");
    } else {
        remove("database/temp_products.txt");
        printf("Produk dengan ID %d tidak ditemukan.\n", productId);
    }
}

int getItemPrice(int itemId) {
    FILE *file = fopen("database/items.txt", "r");
    if (!file) {
        printf("Error opening items.txt file!\n");
        return 0;
    }

    Item item;
    while (fscanf(file, "%d|%99[^|]|%d\n", &item.itemId, item.name, &item.price) == 3) {
        if (item.itemId == itemId) {
            fclose(file);
            return item.price;
        }
    }

    fclose(file);
    return 0;
}

void processReturn() {
    FILE *returnFile = fopen("database/returns.txt", "r+");
    FILE *userFile = fopen("database/users.txt", "r+");
    if (!returnFile || !userFile) {
        printf("Error opening returns or users file!\n");
        return;
    }

    char buffer[256];
    bool hasPendingReturns = false;

    printf("\n--- Daftar Return Pending ---\n");
    while (fgets(buffer, sizeof(buffer), returnFile)) {
        int transactionId, userId, itemId;
        char returnDate[26], reason[200], status[20];

        sscanf(buffer, "%d|%d|%d|%[^|]|%[^|]|%s\n", 
               &transactionId, &userId, &itemId, returnDate, reason, status);

        if (strcmp(status, "Pending") == 0) {
            hasPendingReturns = true;
            printf("Transaction ID: %d\n", transactionId);
            printf("User ID: %d\n", userId);
            printf("Item ID: %d\n", itemId);
            printf("Return Date: %s\n", returnDate);
            printf("Reason: %s\n", reason);
            printf("-----------------------------\n");
        }
    }

    if (!hasPendingReturns) {
        printf("Tidak ada return yang pending.\n");
        fclose(returnFile);
        fclose(userFile);
        return;
    }

    int transactionId;
    printf("Masukkan Transaction ID untuk diproses: ");
    scanf("%d", &transactionId);
    while (getchar() != '\n');

    rewind(returnFile);

    char newReturnFileContent[1024] = "";
    bool returnProcessed = false;

    while (fgets(buffer, sizeof(buffer), returnFile)) {
        int transId, userId, itemId;
        char returnDate[26], reason[200], status[20];

        sscanf(buffer, "%d|%d|%d|%[^|]|%[^|]|%s\n", 
               &transId, &userId, &itemId, returnDate, reason, status);

        if (transId == transactionId && strcmp(status, "Pending") == 0) {
            printf("Pilih aksi untuk transaksi ini:\n");
            printf("1. Terima Return (Accepted)\n");
            printf("2. Tolak Return (Rejected)\n");
            int action;
            scanf("%d", &action);
            while (getchar() != '\n');

            if (action == 1) {
                rewind(userFile);
                char userBuffer[256];
                char newUserFileContent[1024] = "";

                while (fgets(userBuffer, sizeof(userBuffer), userFile)) {
                    int uId, saldo;
                    char username[50], email[50], city[50], phone[20];

                    sscanf(userBuffer, "%d|%[^|]|%[^|]|%[^|]|%[^|]|%d\n", 
                           &uId, username, email, city, phone, &saldo);

                    if (uId == userId) {
                        saldo += getItemPrice(itemId);
                        snprintf(userBuffer, sizeof(userBuffer), 
                                 "%d|%s|%s|%s|%s|%d\n", uId, username, email, city, phone, saldo);
                    }
                    strcat(newUserFileContent, userBuffer);
                }

                freopen("database/users.txt", "w", userFile);
                fputs(newUserFileContent, userFile);
                fclose(userFile);

                snprintf(buffer, sizeof(buffer), 
                         "%d|%d|%d|%s|%s|Accepted\n", 
                         transId, userId, itemId, returnDate, reason);
                printf("Return diterima. Saldo user telah dikembalikan.\n");
            } else if (action == 2) {
                snprintf(buffer, sizeof(buffer), 
                         "%d|%d|%d|%s|%s|Rejected\n", 
                         transId, userId, itemId, returnDate, reason);
                printf("Return ditolak.\n");
            }
            returnProcessed = true;
        }

        strcat(newReturnFileContent, buffer);
    }

    fclose(returnFile);

    if (!returnProcessed) {
        printf("Transaction ID tidak valid atau bukan return pending.\n");
        return;
    }

    FILE *returnFileWrite = fopen("database/returns.txt", "w");
    fputs(newReturnFileContent, returnFileWrite);
    fclose(returnFileWrite);
}



int mainSellerPage() {
    clearScreen();
    printf("WELCOME TO SELLER MENU\n");
    char *main[] = {"Display All Products", "Add Product", "Update Product Stock", "Return Request", "Exit", NULL};
Main:
    printMenu(main);
    int choice = getUserChoice(4);

    switch (choice) {
        case 1:
            displayAllProducts();
            goto Main;
            break;
        case 2:
            addProducts();
            goto Main;
            break;
        case 3:
            updateProductStock();
            goto Main;
            break;
        case 4:
            clearScreen();
            processReturn();    
        case 5:
            return 0;
    }

    return 0;
}
