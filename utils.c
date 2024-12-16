#define _XOPEN_SOURCE 700
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "utils.h"
#include "ui.h"

// GLOBAL UTILITIES
void clearScreen() {
   #ifdef _WIN32
      system("cls");
   #else
      system("clear");
   #endif
}

void printMenu(char *menu[]) {
   int i = 0;
   while (menu[i] != NULL) {
      printf("%d. %s\n", i + 1, menu[i]);
      i++;
   }
}

int getUserChoice(int max) {
   char input[10];
   int choice;
   while (1) {
      printf("(1-%d) ?> ", max);
      if (fgets(input, sizeof(input), stdin)) {
         input[strcspn(input, "\n")] = '\0';
         if (sscanf(input, "%d", &choice) == 1 && choice >= 1 && choice <= max) {
            return choice;
         }
      }
   }
}

void removeNewline(char *str) {
   str[strcspn(str, "\n")] = '\0';
}

char* getCurrentTime() {
   time_t t;
   struct tm *tm_info;
   static char timeString[20];

   time(&t);
   tm_info = localtime(&t);
   strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", tm_info);

   return timeString;
}

int strcasecmp(const char *s1, const char *s2) {
   while (*s1 && *s2) {
      char c1 = tolower((unsigned char)*s1);
      char c2 = tolower((unsigned char)*s2);
      if (c1 != c2) return c1 - c2;
      s1++;
      s2++;
   }
   return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

// USER TABLE
void addRow(Table *table, int id, int colCount, char *values[]) {
   table->rows = realloc(table->rows, (table->rowCount + 1) * sizeof(Row));
   if (!table->rows) {
      perror("error");
      exit(EXIT_FAILURE);
   }

   Row *newRow = &table->rows[table->rowCount];
   newRow->id = id;
   newRow->colCount = colCount;
   newRow->data = malloc(colCount * sizeof(void*));
   if (!newRow->data) {
      perror("error");
      exit(EXIT_FAILURE);
   }

   for (int i = 0; i < colCount; i++) {
      newRow->data[i] = strdup(values[i]);
      if (!newRow->data[i]) {
         perror("error");
         exit(EXIT_FAILURE);
      }
   }

   table->rowCount++;
}

void printTable(Table *table) {
   printf("\nTable Content:\n");
   for (int i = 0; i < table->rowCount; i++) {
      printf("%d\t", table->rows[i].id);
      for (int j = 0; j < table->rows[i].colCount; j++) {
         printf("%s\t", (char *)table->rows[i].data[j]);
      }
      printf("\n");
   }
}

void freeTable(Table *table) {
   for (int i = 0; i < table->rowCount; i++) {
      for (int j = 0; j < table->rows[i].colCount; j++) {
         free(table->rows[i].data[j]);
      }
      free(table->rows[i].data);
   }
   free(table->rows);
   table->rows = NULL;
   table->rowCount = 0;
}

void saveTableToFile(Table *table, char *filename) {
   FILE *ptr = fopen(filename, "w");
   if (!ptr) {
      printf("error open %s file!", filename);
      return;
   }

   for (int i = 0; i < table->rowCount; i++) {
      fprintf(ptr, "%d|", table->rows[i].id);
      for (int j = 0; j < table->rows[i].colCount; j++) {
         fprintf(ptr, "%s", (char *)table->rows[i].data[j]);
         if (j < table->rows[i].colCount - 1) {
            fprintf(ptr, "|");
         }
      }
      fprintf(ptr, "\n");
   }

   fclose(ptr);
}

void readFileToTable(char *filename, Table *table) {
   char buffer[128];
   FILE *ptr = fopen(filename, "r");
   if (!ptr) {
      printf("error open %s file!", filename);
      return;
   }

   while (fgets(buffer, sizeof(buffer), ptr)) {
      buffer[strcspn(buffer, "\n")] = '\0';
      char *idStr = strtok(buffer, "|");
      if (!idStr) {
         printf("error format\n");
         continue;
      }

      char *values[USER_COLS];
      int id = atoi(idStr);

      int colCount = 0;
      char *token = strtok(NULL, "|");
      while (token && colCount < 5) {
         values[colCount++] = token;
         token = strtok(NULL, "|");
      }

      if (colCount != 5) {
         printf("Invalid row format: %s\n", buffer);
         continue;
      }

      addRow(table, table->rowCount + 1, colCount, values);
   }

   fclose(ptr);
}

void readFile(char *filename) {
   char buffer[128];
   FILE *ptr = fopen(filename, "r");
   if (!ptr) {
      printf("error open file!");
      return;
   }

   while (fgets(buffer, sizeof(buffer), ptr) != NULL) {
      printf("%s\n", buffer);
   }

   fclose(ptr);
}

// SIGN UP
void inputUser(Table *table) {
   char username[50], password[50], email[100], address[100], balance[20];

   clearScreen();
   printf("Lets Create Your Account\n-------------------------\n");

   printf("enter username: ");
   fgets(username, sizeof(username), stdin);
   username[strcspn(username, "\n")] = '\0';

   printf("enter password: ");
   fgets(password, sizeof(password), stdin);
   password[strcspn(password, "\n")] = '\0';

   printf("enter email: ");
   fgets(email, sizeof(email), stdin);
   email[strcspn(email, "\n")] = '\0';

   printf("enter address: ");
   fgets(address, sizeof(address), stdin);
   address[strcspn(address, "\n")] = '\0';

   printf("enter balance: ");
   scanf("%s", balance);
   getchar();

   char *values[] = {username, password, email, address, balance};
   readFileToTable(USERS_FILE, table);
   addRow(table, table->rowCount + 1, USER_COLS, values);
   printTable(table);
}

// SIGN IN
void activeUser(char *username) {
   FILE *ptr = fopen(ACTIVE_FILE, "w");
   if (!ptr) {
      printf("error open file");
      return;
   }

   fprintf(ptr, "user status: %s\n", "logged in");
   fprintf(ptr, "username: %s", username);
   fclose(ptr);
}

int validateLogin(char *username, char *password) {
   FILE *ptr = fopen(USERS_FILE, "r");
   if (!ptr) {
      perror("Error opening users file");
      return 0;
   }

   char buffer[256];
   while (fgets(buffer, sizeof(buffer), ptr)) {
      removeNewline(buffer);
      char *fileId = strtok(buffer, "|");
      char *fileUsername = strtok(NULL, "|");
      char *filePassword = strtok(NULL, "|");

      if (fileUsername && filePassword) {
         if (strcmp(username, fileUsername) == 0 && strcmp(password, filePassword) == 0) {
            fclose(ptr);
            return 1;
         }
      }
   }

   fclose(ptr);
   return 0;
}


User *login(Table *table) {
   User *loginUser = malloc(sizeof(User));
   loginUser->name = malloc(128 * sizeof(char));
   loginUser->password = malloc(128 * sizeof(char));
   loginUser->email = malloc(128 * sizeof(char));
   loginUser->address = malloc(128 * sizeof(char));
   loginUser->saldo = 0;

   while (1) {
      printf("Log In Page\n--------------------\n");
      printf("Username: ");
      fgets(loginUser->name, 128, stdin);
      removeNewline(loginUser->name);

      printf("Password: ");
      fgets(loginUser->password, 128, stdin);
      removeNewline(loginUser->password);

      if (validateLogin(loginUser->name, loginUser->password)) {
         FILE *file = fopen(USERS_FILE, "r");
         if (file == NULL) {
            printf("Error opening users file!\n");
            free(loginUser);
            return NULL;
         }

         char buffer[256];
         while (fgets(buffer, sizeof(buffer), file)) {
            char *fileId = strtok(buffer, "|");
            char *fileUsername = strtok(NULL, "|");

            if (strcmp(fileUsername, loginUser->name) == 0) {
               loginUser->id = atoi(fileId);
               loginUser->name = strdup(fileUsername);
               loginUser->password = strdup(strtok(NULL, "|"));
               loginUser->email = strdup(strtok(NULL, "|"));
               loginUser->address = strdup(strtok(NULL, "|"));
               loginUser->saldo = atoi(strtok(NULL, "|"));
               fclose(file);

               activeUser(loginUser->name);
               return loginUser;
            }
         }

         fclose(file);
      } else {
         printf("Invalid username or password. Please try again.\n");
      }
   }
}


void logOut(char *username) {
   FILE *ptr = fopen(ACTIVE_FILE, "w");
   if (!ptr) {
      printf("error open file");
      return;
   }

   fprintf(ptr, "user status: %s\n", "not logged in");
   fclose(ptr);
}

void freeUser(User *user) {
   if (user) {
      free(user->name);
      free(user->password);
      free(user->email);
      free(user->address);
      free(user);
   }
}


// SEARCH PRODUCTS
int readProductsFromFile(const char *filename, Product products[]) {
   FILE *file = fopen(filename, "r");
   if (!file) {
      printf("Error opening file: %s\n", filename);
      return 0;
   }

   int productCount = 0;
   char line[512];

   while (fgets(line, sizeof(line), file) && productCount < MAX_PRODUCTS) {
      char *token = strtok(line, "|");
      if (token == NULL) continue;

      products[productCount].id = atoi(token);

      token = strtok(NULL, "|");
      if (token == NULL) continue;
      strcpy(products[productCount].name, token);

      token = strtok(NULL, "|");
      if (token == NULL) continue;
      strcpy(products[productCount].category, token);

      token = strtok(NULL, "|");
      if (token == NULL) continue;
      products[productCount].price = atoi(token);

      token = strtok(NULL, "|");
      if (token == NULL) continue;
      products[productCount].weight = atoi(token);

      token = strtok(NULL, "|");
      if (token == NULL) continue;
      products[productCount].stock = atoi(token);

      token = strtok(NULL, "|");
      if (token == NULL) continue;
      products[productCount].sold = atoi(token);

      token = strtok(NULL, "|");
      if (token == NULL) continue;
      strcpy(products[productCount].description, token);

      productCount++;
   }

   fclose(file);
   return productCount;
}

void searchProduct(Product products[], int productCount) {
   char keyword[100];

   printf("Search product: ");
   fgets(keyword, sizeof(keyword), stdin);
   keyword[strcspn(keyword, "\n")] = '\0';

   printf("\nSearch Results:\n");
   int found = 0;
   for (int i = 0; i < productCount; i++) {
      if (strstr(products[i].name, keyword) != NULL || strstr(products[i].description, keyword) != NULL) {
         printf("ID: %d\n", products[i].id);
         printf("Name: %s\n", products[i].name);
         printf("Category: %s\n", products[i].category);
         printf("Price: %d\n", products[i].price);
         printf("Weight: %d\n", products[i].weight);
         printf("Stock: %d\n", products[i].stock);
         printf("Description: %s\n\n", products[i].description);
         found = 1;
      }
   }

   if (!found) {
      printf("No products found matching your search.\n");
   }
}

// DISPLAY ALL PRODUCTS
void displayAllProducts() {
   FILE *file = fopen(PRODUCTS_FILE, "r");
   if (!file) {
      printf("Error: Tidak dapat membuka file %s\n", PRODUCTS_FILE);
      return;
   }

   Product product;
   char buffer[256];

   printf("\n--- Daftar Produk ---\n");
   while (fgets(buffer, sizeof(buffer), file)) {
      sscanf(buffer, "%d|%[^|]|%[^|]|%d|%d|%d|%d|%[^\n]", 
            &product.id, product.name, product.category, &product.price, 
            &product.weight, &product.stock, &product.sold, product.description);

      printf("ID: %d\n", product.id);
      printf("Nama: %s\n", product.name);
      printf("Kategori: %s\n", product.category);
      printf("Harga: %d\n", product.price);
      printf("Berat: %d gram\n", product.weight);
      printf("Stok: %d\n", product.stock);
      printf("Terjual: %d\n", product.sold);
      printf("Deskripsi: %s\n", product.description);
      printf("-----------------------------\n");
   }

   fclose(file);
}

void displayProductsByCategory() {
   clearScreen();
   printf("--- Produk Berdasarkan Kategori ---\n");

   FILE *file = fopen(PRODUCTS_FILE, "r");
   if (!file) {
      printf("Error: Tidak dapat membuka file %s\n", PRODUCTS_FILE);
      return;
   }

   char categories[100][50];
   int categoryCount = 0;
   Product products[1000];
   int productCount = 0;
   char buffer[256];

   while (fgets(buffer, sizeof(buffer), file)) {
      sscanf(buffer, "%d|%[^|]|%[^|]|%d|%d|%d|%d|%[^\n]", &products[productCount].id, products[productCount].name, products[productCount].category, &products[productCount].price, &products[productCount].weight, &products[productCount].stock, &products[productCount].sold, products[productCount].description);

      bool found = false;
      for (int i = 0; i < categoryCount; i++) {
         if (strcmp(categories[i], products[productCount].category) == 0) {
               found = true;
               break;
         }
      }

      if (!found) {
         strcpy(categories[categoryCount], products[productCount].category);
         categoryCount++;
      }
      productCount++;
   }
   fclose(file);

   printf("Daftar Kategori:\n");
   for (int i = 0; i < categoryCount; i++) {
      printf("%d. %s\n", i + 1, categories[i]);
   }

   int choice = getUserChoice(categoryCount);

   char *selectedCategory = categories[choice - 1];
   printf("\nProduk dalam kategori '%s':\n", selectedCategory);

   for (int i = 0; i < productCount; i++) {
      if (strcmp(products[i].category, selectedCategory) == 0) {
         printf("ID: %d\n", products[i].id);
         printf("Nama: %s\n", products[i].name);
         printf("Harga: %d\n", products[i].price);
         printf("Berat: %d\n", products[i].weight);
         printf("Stok: %d\n", products[i].stock);
         printf("Deskripsi: %s\n", products[i].description);
         printf("-----------------------------\n");
      }
   }
}


// CART
void addToCart(const char *username, int product_id, const char *product_name, const char *productDesc, int price, int quantity, int weight) {
   int itemid = 1;
   FILE *file = fopen("database/cart.txt", "r");
   if (file != NULL) {
      char line[256];
      while (fgets(line, sizeof(line), file)) {
         char cartUsername[100];
         int current_itemid, product_id_read, quantity_read, price_read, weightRead;
         char product_name_read[100], productDescRead[255];

         sscanf(line, "%[^|]|%d|%d|%d|%d|%d|%[^|]|%[^|]", cartUsername, &current_itemid, &product_id_read, &quantity_read, &price_read, &weightRead, product_name_read, productDescRead);

         if (strcmp(cartUsername, username) == 0) {
            if (current_itemid >= itemid) {
               itemid = current_itemid + 1;
            }
         }
      }
      fclose(file);
   }

   file = fopen("database/cart.txt", "a");
   if (file == NULL) {
      printf("Error opening cart.txt file!\n");
      return;
   }

   int totalHarga = price * quantity;
   fprintf(file, "%s|%d|%d|%d|%d|%d|%s|%s\n", username, itemid, product_id, quantity, totalHarga, weight, product_name, productDesc);
   fclose(file);

   printf("Product added to cart with item ID: %d!\n", itemid);
   clearScreen();
}

int viewCart(const char *username) {
   FILE *file = fopen("database/cart.txt", "r");
   if (file == NULL) {
      printf("Error opening cart.txt file!\n");
      return 0;
   }

   char line[256];
   printf("\nCart for %s:\n", username);
   int found = 0;

   while (fgets(line, sizeof(line), file)) {
      char cartUsername[100], product_name[100], productDesc[255];
      int product_id, item_id, quantity, price, weight;

      sscanf(line, "%[^|]|%d|%d|%d|%d|%d|%[^|]|%[^|]", cartUsername, &item_id, &product_id, &quantity, &price, &weight, product_name, productDesc);

      removeNewline(productDesc);
      if (strcmp(cartUsername, username) == 0) {
         printf("---------------------------\n");
         printf("Item ID: %d\n", item_id);
         printf("Product ID: %d\n", product_id);
         printf("Product Name: %s\n", product_name);
         printf("Product Desc: %s\n", productDesc);
         printf("Quantity: %d\n", quantity);
         printf("Total Price: %d\n", price);
         printf("---------------------------\n");
         found = 1;
      }
   }

   if (!found) {
      printf("No products in cart.\n\n");
      return found;
   }

   return found;
   fclose(file);
}

bool isItemInCart(const char *username, int itemid) {
   FILE *file = fopen("database/cart.txt", "r");
   if (file == NULL) {
      printf("Error opening cart.txt file!\n");
      return false;
   }

   char line[256];
   while (fgets(line, sizeof(line), file)) {
      char cartUsername[100];
      int current_itemid, product_id, quantity, total_price, weight;
      char product_name[100], productDesc[255];

      sscanf(line, "%[^|]|%d|%d|%d|%d|%d|%[^|]|%[^|]", cartUsername, &current_itemid, &product_id, &quantity, &total_price, &weight, product_name, productDesc);

      if (strcmp(cartUsername, username) == 0 && current_itemid == itemid) {
         fclose(file);
         return true;
      }
   }

   fclose(file);
   return false;
}

// TRANSACTION - CHECKOUT
int loadCityData(const char *filename, CityDistance cityData[]) {
   FILE *file = fopen(filename, "r");
   if (!file) {
      printf("Error opening file %s\n", filename);
      return -1;
   }

   int i = 0;
   while (fscanf(file, "%49[^|]|%d\n", cityData[i].city, &cityData[i].distance) == 2) {
      i++;
      if (i >= MAX_CITIES) break;
   }

   fclose(file);
   return i;
}

int getDistance(CityDistance city_data[], int num_cities, const char *city) {
   for (int i = 0; i < num_cities; i++) {
      if (strcasecmp(city_data[i].city, city) == 0) {
         return city_data[i].distance;
      }
   }
   return -1;
}

int calculateTotalPrice(const char *username, int *itemID, int itemCount) {
   FILE *cartFile = fopen("database/cart.txt", "r");
   if (!cartFile) {
      printf("Error opening cart file!\n");
      return 0; 
   }

   char line[256];
   int totalPrice = 0;

   while (fgets(line, sizeof(line), cartFile)) {
      char user[100], productName[100], productDesc[255];
      int itemIDNow, productID, quantity, price;

      sscanf(line, "%[^|]|%d|%d|%d|%d|%[^|]|%[^|]", user, &itemIDNow, &productID, &quantity, &price, productName, productDesc);

      if (strcmp(user, username) == 0) {
         for (int i = 0; i < itemCount; i++) {
            if (itemID[i] == itemIDNow) {
               totalPrice += price;
            }
         }
      }
   }

   fclose(cartFile);
   return totalPrice;
}

int calculateTotalPostage(int distance, const char *username, int *itemID, int itemCount) {
   FILE *cartFile = fopen("database/cart.txt", "r");
   if (!cartFile) {
      printf("Error opening cart file!\n");
      return 0;      
   }

   char line[256];
   int totalPostage = TARIF_PER_KM * distance;

   while (fgets(line, sizeof(line), cartFile)) {
      char user[100], productName[100], productDesc[255];
      int itemIDNow, productID, quantity, price, weight;

      sscanf(line, "%[^|]|%d|%d|%d|%d|%d|%[^|]|%[^|]", user, &itemIDNow, &productID, &quantity, &price, &weight, productName, productDesc);

      if (strcmp(user, username) == 0) {
         for (int i = 0; i < itemCount; i++) {
            if (itemID[i] == itemIDNow) {
               totalPostage += TARIF_PER_KG * weight;
            }
         }
      }
   }

   fclose(cartFile);
   return totalPostage;
}

int getNewTransactionId() {
   FILE *file = fopen("database/transactions.txt", "r");
   if (!file) {
      printf("Error opening transactions.txt file!\n");
      return -1;
   }

   int lastId = 0;
   char buffer[256];

   while (fgets(buffer, sizeof(buffer), file)) {
      char *token = strtok(buffer, "|");
      int id = atoi(token);
      if (id > lastId) {
         lastId = id;
      }
   }

   fclose(file);
   return lastId + 1;
}

const char* handleStatus(time_t transactionTime) {
   time_t currentTime = time(NULL);
   double secondsElapsed = difftime(currentTime, transactionTime);

   if (secondsElapsed <= 5 * 3600) {
      return STATUS_WAITING;
   }
   else if (secondsElapsed <= 24 * 3600) {
      return STATUS_PACKING;
   }
   else if (secondsElapsed <= 60 * 3600) {
      return STATUS_SHIPPED;
   }
   else {
      return STATUS_DELIVERED;
   }
}

void saveTransaction(int transactionId, User *userActive, int distance, char *jasa) {
   FILE *file = fopen("database/transactions.txt", "a");
   if (!file) {
      printf("Error opening transactions.txt file!\n");
      return;
   }

   time_t currentTime = time(NULL);
   char timeString[26];
   ctime_r(&currentTime, timeString);
   timeString[strlen(timeString) - 1] = '\0';

   const char *status = handleStatus(currentTime);

   int estimateDays = 5;
   if (distance <= 0) {
      estimateDays = 5;
   } else if (distance <= 1000) {
      estimateDays = 5 + (distance / 100);
   } else {
      estimateDays = 10 + (distance / 200);
   }

   struct tm *timeInfo = localtime(&currentTime);
   timeInfo->tm_mday += estimateDays;
   mktime(timeInfo);

   char estimateTimeString[26];
   strftime(estimateTimeString, sizeof(estimateTimeString), "%a %b %d %H:%M:%S %Y", timeInfo);

   fprintf(file, "%d|%d|%s|%s|%s|%s\n", 
         transactionId, userActive->id, timeString, status, estimateTimeString, jasa);

   fclose(file);
}

void updateUserSaldo(User *userActive) {
   FILE *file = fopen(USERS_FILE, "r");
   if (file == NULL) {
      printf("Error opening users.txt file!\n");
      return;
   }

   FILE *tempFile = fopen("temp_users.txt", "w");
   if (tempFile == NULL) {
      printf("Error creating temporary file!\n");
      fclose(file);
      return;
   }

   char buffer[256];
   int updated = 0;

   while (fgets(buffer, sizeof(buffer), file)) {
      char *fileID = strtok(buffer, "|");
      char *fileUsername = strtok(NULL, "|");
      char *filePassword = strtok(NULL, "|");
      char *fileEmail = strtok(NULL, "|");
      char *fileAddress = strtok(NULL, "|");
      char *fileSaldo = strtok(NULL, "\n");

      if (strcmp(fileUsername, userActive->name) == 0) {
         fprintf(tempFile, "%d|%s|%s|%s|%s|%d\n", userActive->id, userActive->name, userActive->password, userActive->email, userActive->address, userActive->saldo);
         updated = 1;
      } else {
         fprintf(tempFile, "%s|%s|%s|%s|%s|%s\n", fileID, fileUsername, filePassword, fileEmail, fileAddress, fileSaldo);
      }
   }

   fclose(file);
   fclose(tempFile);

   if (updated) {
      remove(USERS_FILE);
      rename("temp_users.txt", USERS_FILE);
   } else {
      remove("temp_users.txt");
   }
}

void saveItems(int transactionId, User *userActive, int *itemID, int itemCount) {
   FILE *file = fopen("database/items.txt", "a");
   if (!file) {
      printf("Error opening items.txt file!\n");
      return;
   }

   FILE *cartFile = fopen("database/cart.txt", "r");
   if (!cartFile) {
      printf("Error opening cart.txt file!\n");
      fclose(file);
      return;
   }

   char buffer[256];
   fprintf(file, "%d {\n", transactionId);

   while (fgets(buffer, sizeof(buffer), cartFile)) {
      char *token = strtok(buffer, "|");
      char *user = token;
      token = strtok(NULL, "|");
      char *idItem = token;
      token = strtok(NULL, "|");
      char *productID = token;
      token = strtok(NULL, "|");
      char *qty = token;
      token = strtok(NULL, "|");
      char *totalPrice = token;
      token = strtok(NULL, "|");
      char *weight = token;
      token = strtok(NULL, "|");
      char *namaBarang = token;
      token = strtok(NULL, "|");
      char *productDesc = token;
      removeNewline(productDesc);

      if(!strcmp(user, userActive->name)) {
         for (int i = 0; i < itemCount; i++) {
            if (atoi(idItem) == itemID[i]) {
               fprintf(file, "    %s|%s|%s|%s|%s\n", productID, qty, totalPrice, namaBarang, productDesc);
               break;
            }
         }
      }
   }

   fprintf(file, "}\n");

   fclose(cartFile);
   fclose(file);
}

void removeItemsFromCart(const char *username, int *itemID, int itemCount) {
   FILE *file = fopen("database/cart.txt", "r");
   FILE *tempFile = fopen("database/cart_temp.txt", "w");
   if (!file || !tempFile) {
      printf("Error opening cart file!\n");
      return;
   }

   char buffer[256];
   int newItemID = 1;

   while (fgets(buffer, sizeof(buffer), file)) {
      char cartOwner[50], productName[100], description[200];
      int cartItemID, productID, qty, price, weight;

      sscanf(buffer, "%[^|]|%d|%d|%d|%d|%d|%[^|]|%[^\n]", 
            cartOwner, &cartItemID, &productID, &qty, &price, &weight, productName, description);

      bool shouldDelete = false;
      if (strcmp(cartOwner, username) == 0) {
         for (int i = 0; i < itemCount; i++) {
               if (cartItemID == itemID[i]) {
                  shouldDelete = true;
                  break;
               }
         }
      }

      if (!shouldDelete) {
         fprintf(tempFile, "%s|%d|%d|%d|%d|%d|%s|%s\n", 
                  cartOwner, newItemID++, productID, qty, price, weight, productName, description);
      }
   }

   fclose(file);
   fclose(tempFile);

   remove("database/cart.txt");
   rename("database/cart_temp.txt", "database/cart.txt");
}


void checkout(User *userActive) {
Order:
   printf("\n--- Checkout ---\n");
   if (!viewCart(userActive->name)) {
      return;
   };

   int *itemID = (int*) malloc(10 * sizeof(int));
   int tmp, itemCount = 0;
   char isAgain;
   while (true) {
      printf("\nItem ID in cart to checkout ");
      tmp = getUserChoice(100);

      if (isItemInCart(userActive->name, tmp)) {
         itemID[itemCount++] = tmp;
      } else {
         printf("Invalid Item ID. This item is not in your cart.\n");
         continue;
      }

      printf("Do you want to add another item? (y/n)  ");
      scanf(" %c", &isAgain);
      while (getchar() != '\n');

      if ((isAgain == 'n' || isAgain == 'N') && (isAgain != 'y' || isAgain != 'Y')) {
         clearScreen();
         printf("Proceeding to checkout...\n");
         break;
      }
   }

   CityDistance cityData[MAX_CITIES];
   char city[50];
   int numCities = loadCityData("database/city.txt", cityData);
   int distance;

   while (true) {
      printf("Enter destination city: ");
      scanf("%s", city);
      while (getchar() != '\n');
      distance = getDistance(cityData, numCities, city);

      if (distance != -1) break;
   }

   char *menuJasa[] = {JNE_KILAT, JNE, JNT, JMK, NULL};
   clearScreen();
   printf("Pilih jasa pengiriman\n");
   printMenu(menuJasa);
   int userJasa = getUserChoice(4);
   char *jasa;

   switch (userJasa) {
      case 1:
         jasa = JNE_KILAT;
         break;
      case 2:
         jasa = JNE;
         break;
      case 3:
         jasa = JNT;
         break;
      case 4:
         jasa = JMK;
         break;

      default:
         break;
   }

   int postage = calculateTotalPostage(distance, userActive->name, itemID, itemCount);
   int totalPrice = calculateTotalPrice(userActive->name, itemID, itemCount);
   int totalPayment = totalPrice + postage;

   if (userActive->saldo < totalPayment) {
      printf("Insufficient balance for the transaction.\n");
      return;
   } else {
      clearScreen();
      displayReceipt(userActive, itemID, itemCount, city, distance);

Confirm:
      printf("your password to confirm (q to cancel, b to back): ");
      char confirm[255];
      fgets(confirm, sizeof(confirm), stdin);
      removeNewline(confirm);
      if (!strcmp(confirm, "q")) {
         clearScreen();
         printf("transaction canceled\n");
         return;
      } else if (!strcmp(confirm, "b")) {
         clearScreen();
         goto Order;
      } else if (strcmp(confirm, userActive->password)) {
         printf("transaction failed, password incorrect\n");
         goto Confirm;
      }

      userActive->saldo -= totalPayment;
      updateUserSaldo(userActive);

      int transactionId = getNewTransactionId();

      saveTransaction(transactionId, userActive, distance, jasa);
      saveItems(transactionId, userActive, itemID, itemCount);
      removeItemsFromCart(userActive->name, itemID, itemCount);
      clearScreen();
      printf("Transaction completed! Order will arrive in 3-5 days.\n");
   }

   free(itemID);
}

void displayAllTransaction(User *userActive) {
   FILE *file = fopen("database/transactions.txt", "r");
   if (!file) {
         printf("Error opening transactions.txt file!\n");
         return;
   }

   printf("\n--- Semua Transaksi Anda ---\n");
   char buffer[256];
   while (fgets(buffer, sizeof(buffer), file)) {
      int transactionId, userId, estimateDelivery;
      char timeString[26], status[20];

      sscanf(buffer, "%d|%d|%[^|]|%[^|]|%d\n", &transactionId, &userId, timeString, status, &estimateDelivery);

      if (userId == userActive->id) {
         printf("ID Transaksi: %d\n", transactionId);
         printf("Waktu: %s\n", timeString);
         printf("Status: %s\n", status);
         printf("Estimasi Pengiriman: H+%d\n", estimateDelivery);
         printf("-----------------------------\n");
      }
   }
   fclose(file);
}

// ora dinggo
void saveReturnRequest(int transactionId, User *userActive, int productID, const char *productName, int quantity, const char *reason, int totalPrice) {
   FILE *file = fopen("database/returns.txt", "a");
   if (!file) {
      printf("Error opening return.txt file!\n");
      return;
   }

   fprintf(file, "%d|%d|%d|%d|%s|%s|%s|%d\n", transactionId, userActive->id, quantity, productID, productName, reason, "Selesai", totalPrice);

   fclose(file);
}

// ora diggo iki
void returnMenu(User *userActive) {
   printf("\n--- Return Barang ---\n");

   printf("Masukkan ID Transaksi yang ingin Anda return: ");
   int transactionId = getUserChoice(1000);

   FILE *file = fopen("database/transactions.txt", "r");
   if (!file) {
      printf("Error opening transactions.txt file!\n");
      return;
   }

   int found = 0;
   char buffer[256], status[20];
   while (fgets(buffer, sizeof(buffer), file)) {
      int currentTransactionId, userId;
      sscanf(buffer, "%d|%d|%*[^|]|%[^|]|%*d", &currentTransactionId, &userId, status);

      if (currentTransactionId == transactionId && userId == userActive->id) {
         if (strcmp(status, "Selesai") == 0) {
            found = 1;
         } else {
            printf("Transaksi dengan ID %d belum selesai atau belum sampai. Tidak dapat di-return.\n", transactionId);
            fclose(file);
            return;
         }
         break;
      }
   }
   fclose(file);

   if (!found) {
      printf("Transaksi dengan ID %d tidak ditemukan atau bukan milik Anda.\n", transactionId);
      return;
   }

   FILE *itemsFile = fopen("database/items.txt", "r");
   if (!itemsFile) {
      printf("Error opening items.txt file!\n");
      return;
   }

   printf("\nBarang dalam transaksi ini:\n");
   int insideTransaction = 0;
   char productName[100], description[100];
   int qty, totalPrice, productID;
   while (fgets(buffer, sizeof(buffer), itemsFile)) {
      if (strstr(buffer, "{") && atoi(buffer) == transactionId) {
         insideTransaction = 1;
         continue;
      }

      if (strstr(buffer, "}") && insideTransaction) {
         insideTransaction = 0;
         break;
      }

      if (insideTransaction) {
         sscanf(buffer, "%d|%d|%d|%[^|]|%[^\n]", &productID, &qty , &totalPrice, productName, description);

         printf("ID Barang: %d\n", productID);
         printf("Nama Barang: %s\n", productName);
         printf("Harga: %d\n", totalPrice);
         printf("Jumlah: %d\n", qty);
         printf("Deskripsi: %s\n", description);
         printf("-----------------------------\n");
      }
   }
   fclose(itemsFile);

   char reason[255];
   printf("Apa alasan anda untuk mengembalikan barang ini: ");
   fgets(reason, sizeof(reason), stdin);
   removeNewline(reason);

   printf("\nApakah Anda yakin ingin melakukan permintaan return? (y/n): ");
   char confirmation;
   scanf(" %c", &confirmation);
   while (getchar() != '\n');

   if (confirmation == 'y' || confirmation == 'Y') {
      removeNewline(productName);
      saveReturnRequest(transactionId, userActive, productID, productName, qty, reason, totalPrice);
      printf("Permintaan return telah dikirimkan ke penjual, tunggu penjual mengonfirmasi permintaan anda.\n");
   } else {
      printf("Permintaan return dibatalkan.\n");
   }
}

void returnItems(User *userActive) {
   FILE *transFile = fopen("database/transactions.txt", "r");
   FILE *returnFile = fopen("database/returns.txt", "a");
   if (!transFile || !returnFile) {
      printf("Error opening transaction or return file!\n");
      return;
   }

   int transactionId;
   char buffer[256];
   bool transactionFound = false;

   printf("\n--- Transaksi Anda ---\n");
   while (fgets(buffer, sizeof(buffer), transFile)) {
      int transId, userId, estimateDays;
      char timeString[26], status[20], deliveryService[20];

      sscanf(buffer, "%d|%d|%[^|]|%[^|]|%d|%s\n", 
            &transId, &userId, timeString, status, &estimateDays, deliveryService);

      if (userId == userActive->id) {
         transactionFound = true;
         printf("Transaction ID: %d\n", transId);
         printf("Waktu: %s\n", timeString);
         printf("Status: %s\n", status);
         printf("-----------------------------\n");
      }
   }

   fclose(transFile);

   if (!transactionFound) {
      printf("Anda tidak memiliki transaksi.\n");
      return;
   }

   printf("Masukkan ID transaksi untuk return: ");
   scanf("%d", &transactionId);
   while (getchar() != '\n');

   FILE *transFileAgain = fopen("database/transactions.txt", "r");
   if (!transFileAgain) {
      printf("Error opening transactions file!\n");
      return;
   }

   bool validReturn = false;
   bool isDelivered = false;

   while (fgets(buffer, sizeof(buffer), transFileAgain)) {
      int transId, userId, estimateDays;
      char timeString[26], status[20], deliveryService[20];

      sscanf(buffer, "%d|%d|%[^|]|%[^|]|%d|%s\n", 
            &transId, &userId, timeString, status, &estimateDays, deliveryService);

      if (transId == transactionId) {
         if (strcmp(status, "Delivered") != 0) {
               printf("Barang belum sampai. Tidak dapat melakukan return.\n");
               fclose(transFileAgain);
               return;
         } else {
               isDelivered = true;
         }

         struct tm transTime = {0};
         strptime(timeString, "%a %b %d %H:%M:%S %Y", &transTime);
         time_t transTimestamp = mktime(&transTime);

         time_t deliveryTimestamp = transTimestamp + (estimateDays * 24 * 60 * 60);

         time_t currentTime = time(NULL);

         double daysSinceDelivery = difftime(currentTime, deliveryTimestamp) / (24 * 60 * 60);
         if (daysSinceDelivery <= 7 && daysSinceDelivery >= 0) {
               validReturn = true;
         }

         break;
      }
   }

   fclose(transFileAgain);

   if (!isDelivered) {
      printf("Barang belum sampai.\n");
      return;
   }

   if (!validReturn) {
      printf("Barang sudah melebihi batas waktu 7 hari untuk return.\n");
      return;
   }

   char reason[200];
   printf("Masukkan alasan return: ");
   fgets(reason, sizeof(reason), stdin);
   reason[strcspn(reason, "\n")] = '\0';

   time_t currentTime = time(NULL);
   char returnDate[26];
   ctime_r(&currentTime, returnDate);
   returnDate[strlen(returnDate) - 1] = '\0';
   fprintf(returnFile, "%d|%d|%d|%s|%s|Pending\n", 
         transactionId, userActive->id, transactionId, returnDate, reason);

   fclose(returnFile);
   printf("Return barang berhasil diajukan! Harap tunggu persetujuan.\n");
}


void refreshTransactionStatus() {
   FILE *file = fopen("database/transactions.txt", "r");
   if (!file) {
      printf("Error opening transactions.txt!\n");
      return;
   }

   FILE *tempFile = fopen("database/transactions_temp.txt", "w");
   if (!tempFile) {
      printf("Error opening transactions_temp.txt!\n");
      fclose(file);
      return;
   }

   char buffer[256];
   while (fgets(buffer, sizeof(buffer), file)) {
      int transactionId, userId, estimateDelivery;
      char timeString[26], status[20];

      sscanf(buffer, "%d|%d|%[^|]|%[^|]|%d", &transactionId, &userId, timeString, status, &estimateDelivery);

      struct tm transactionTimeStruct = {0};
      strptime(timeString, "%a %b %d %H:%M:%S %Y", &transactionTimeStruct);
      time_t transactionTime = mktime(&transactionTimeStruct);

      const char *newStatus = handleStatus(transactionTime);

      fprintf(tempFile, "%d|%d|%s|%s|%d\n", transactionId, userId, timeString, newStatus, estimateDelivery);
   }

   fclose(file);
   fclose(tempFile);

   remove("database/transactions.txt");
   rename("database/transactions_temp.txt", "database/transactions.txt");
}

// DISPLAY PRODUCTS
void mostPurchaseProducts() {
   clearScreen();
   printf("--- Produk Paling Banyak Dibeli ---\n");

   FILE *file = fopen(PRODUCTS_FILE, "r");
   if (!file) {
      printf("Error: Tidak dapat membuka file %s\n", PRODUCTS_FILE);
      return;
   }

   Product *products = malloc(sizeof(Product) * 1000);
   if (!products) {
      printf("Error: Tidak dapat mengalokasikan memori.\n");
      fclose(file);
      return;
   }

   int count = 0;
   char buffer[256];

   while (fgets(buffer, sizeof(buffer), file)) {
      sscanf(buffer, "%d|%[^|]|%[^|]|%d|%d|%d|%d|%[^\n]", &products[count].id, products[count].name, products[count].category, &products[count].price, &products[count].weight, &products[count].stock, &products[count].sold, products[count].description);
      count++;
   }
   fclose(file);

   for (int i = 0; i < count - 1; i++) {
      for (int j = 0; j < count - i - 1; j++) {
         if (products[j].sold < products[j + 1].sold) {
               Product temp = products[j];
               products[j] = products[j + 1];
               products[j + 1] = temp;
         }
      }
   }

   printf("Top 5 Produk Paling Banyak Dibeli:\n");
   for (int i = 0; i < count && i < 5; i++) {
      printf("ID: %d\n", products[i].id);
      printf("Nama: %s\n", products[i].name);
      printf("Kategori: %s\n", products[i].category);
      printf("Harga: %d\n", products[i].price);
      printf("Berat: %d\n", products[i].weight);
      printf("Stok: %d\n", products[i].stock);
      printf("Jumlah Dibeli: %d\n", products[i].sold);
      printf("Deskripsi: %s\n", products[i].description);
      printf("-----------------------------\n");
   }
   free(products);
}

void sortProductPrice() {
   clearScreen();
   printf("--- Urutkan Produk Berdasarkan Harga ---\n");

   FILE *file = fopen(PRODUCTS_FILE, "r");
   if (!file) {
      printf("Error: Tidak dapat membuka file %s\n", PRODUCTS_FILE);
      return;
   }

   Product *products = malloc(sizeof(Product) * 1000);
   if (!products) {
      printf("Error: Tidak dapat mengalokasikan memori.\n");
      fclose(file);
      return;
   }

   int count = 0;
   char buffer[256];

   while (fgets(buffer, sizeof(buffer), file)) {
      sscanf(buffer, "%d|%[^|]|%[^|]|%d|%d|%d|%d|%[^\n]", &products[count].id, products[count].name, products[count].category, &products[count].price, &products[count].weight, &products[count].stock, &products[count].sold, products[count].description);
      count++;
   }
   fclose(file);

   for (int i = 0; i < count - 1; i++) {
      for (int j = 0; j < count - i - 1; j++) {
         if (products[j].price < products[j + 1].price) {
               Product temp = products[j];
               products[j] = products[j + 1];
               products[j + 1] = temp;
         }
      }
   }

   file = fopen(PRODUCTS_FILE, "w");
   for (int i = 0; i < count; i++) {
      fprintf(file, "%d|%s|%s|%d|%d|%d|%d|%s\n", products[i].id, products[i].name, products[i].category, products[i].price, products[i].weight, products[i].stock, products[i].sold, products[i].description);
   }
   fclose(file);

   printf("Produk berhasil diurutkan berdasarkan harga!\n");
   free(products);
}