#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

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
      printf("\n(1-%d) ?> ", max);
      if (fgets(input, sizeof(input), stdin)) {
         input[strcspn(input, "\n")] = '\0';
         if (sscanf(input, "%d", &choice) == 1 && choice >= 1 && choice <= max) {
               return choice;
         }
      }
      printf("Invalid choice. Please try again.\n");
   }
}

void removeNewline(char *str) {
   str[strcspn(str, "\n")] = '\0';
}

// TABLE & DATABASE UTILITIES
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

   while (1) {
      clearScreen();
      printf("Log In Page\n--------------------\n");
      printf("Username: ");
      fgets(loginUser->name, 128, stdin);
      removeNewline(loginUser->name);

      printf("Password: ");
      fgets(loginUser->password, 128, stdin);
      removeNewline(loginUser->password);

      if (validateLogin(loginUser->name, loginUser->password)) {
         activeUser(loginUser->name);
         return loginUser;
      }

      printf("Invalid username or password. Please try again.\n");
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
      products[productCount].weight = atof(token);

      token = strtok(NULL, "|");
      if (token == NULL) continue;
      products[productCount].stock = atoi(token);

      token = strtok(NULL, "|");
      if (token == NULL) continue;
      products[productCount].status = atoi(token);

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

   printf("Enter product name or description to search: ");
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
         printf("Weight: %.2f\n", products[i].weight);
         printf("Stock: %d\n", products[i].stock);
         printf("Description: %s\n\n", products[i].description);
         found = 1;
      }
   }

   if (!found) {
      printf("No products found matching your search.\n");
   }
}

// CART
void addToCart(const char *username, int product_id, const char *product_name, int price, int quantity) {
   FILE *file = fopen("database/cart.txt", "a");
   if (file == NULL) {
      printf("Error opening cart.txt file!\n");
      return;
   }

   fprintf(file, "%s|%d|%s|%d|%d\n", username, product_id, product_name, quantity, price);
   fclose(file);
   printf("Product added to cart!\n");
   clearScreen();
}

void viewCart(const char *username) {
   FILE *file = fopen("database/cart.txt", "r");
   if (file == NULL) {
      printf("Error opening cart.txt file!\n");
      return;
   }

   char line[256];
   printf("\nCart for %s:\n", username);
   int found = 0;

   while (fgets(line, sizeof(line), file)) {
      char cartUsername[100], product_name[100];
      int product_id, quantity, price;

      sscanf(line, "%[^|]|%d|%[^|]|%d|%d", cartUsername, &product_id, product_name, &quantity, &price);

      if (strcmp(cartUsername, username) == 0) {
         printf("Product ID: %d\n", product_id);
         printf("Product Name: %s\n", product_name);
         printf("Quantity: %d\n", quantity);
         printf("Price: %d\n", price);
         printf("---------------------------\n");
         found = 1;
      }
   }

   if (!found) {
      printf("No products in cart.\n");
   }

   fclose(file);
}

// CHECK OUT
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

int calculateTotalPrice(const char *username) {
   FILE *cartFile = fopen("database/cart.txt", "r");
   if (!cartFile) {
      printf("Error opening cart file!\n");
      return 0; 
   }

   char line[256];
   int total = 0;

   while (fgets(line, sizeof(line), cartFile)) {
      char user[100], product_name[100];
      int product_id, quantity, price_total;

      sscanf(line, "%[^|]|%d|%[^|]|%d|%d", user, &product_id, product_name, &quantity, &price_total);

      if (strcmp(user, username) == 0) {
         total += price_total;
      }
   }

   fclose(cartFile);
   return total;
}

void checkout(User *userActive) {
   CityDistance city_data[MAX_CITIES];
   int num_cities = loadCityData("database/city.txt", city_data);
   if (num_cities == -1) {
      printf("Error loading city data. Exiting...\n");
      return;
   }

   printf("\n--- Checkout ---\n");
   printf("Your Cart:\n");
   viewCart(userActive->name);

   char city[50];
   printf("Enter your destination city: ");
   scanf("%s", city);

   int distance = getDistance(city_data, num_cities, city);
   if (distance == -1) {
      printf("City not found. Please try again.\n");
      return;
   }

   int ongkir = distance * TARIF_PER_KM;
   printf("Delivery cost to %s: Rp %d\n", city, ongkir);

   int totalHargaBarang = calculateTotalPrice(userActive->name);  // Fungsi untuk menghitung total harga barang

   int totalPembayaran = totalHargaBarang + ongkir;
   printf("Total payment (including delivery): Rp %d\n", totalPembayaran);

   printf("Do you want to proceed with the payment? (1 for Yes, 0 for No): ");
   int confirm;
   while (scanf("%d", &confirm) != 1 || (confirm != 1 && confirm != 0)) {
      printf("Invalid choice. Enter 1 for Yes or 0 for No: ");
      while (getchar() != '\n');
   }

   if (confirm == 1) {
      printf("Processing payment...\n");
   } else {
      printf("Payment canceled. Returning to main menu...\n");
   }
}
