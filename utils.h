#ifndef UTILS_H
#define UTILS_H

#define USER_COLS 5
#define CART_COLS 2
#define PODUCTS_COLS 6
#define MAX_PRODUCTS 100
#define TARIF_PER_KM 2500
#define MAX_CITIES 10

#define USERS_FILE "database/users.txt"
#define ACTIVE_FILE "database/active.txt"
#define PRODUCTS_FILE "database/products.txt"


// DATABASE
typedef struct {
   int id;
   void **data;
   int colCount;
} Row;

typedef struct {
   Row *rows;
   int rowCount;
} Table;

typedef struct {
   char *name;
   char *password;
} User;

typedef struct {
   int id;
   char name[100];
   char category[50];
   int price;
   float weight;
   int stock;
   int status;
   char description[256];
} Product;

typedef struct {
   char username[100];
   int product_id;
   char product_name[100];
   int quantity;
   int price;
} CartItem;

typedef struct {
   char city[50];
   int distance;
} CityDistance;

// GLOBAL UTILITIES
void clearScreen();
void printMenu(char *menu[]);
void readFile(char *filename);
int getUserChoice(int max);

// ACCOUNT UTILITIES
void addRow(Table *table, int id, int colCountc, char *values[]);
void printTable(Table *table);
void freeTable(Table *table);
void inputUser(Table *table);
void saveTableToFile(Table *table, char *filename);
void readFileToTable(char *filename, Table *table);
User *login(Table *table);
void logOut(char *username);
void freeUser(User *user);

// SEARCH MENU
void searchProduct(Product products[], int productCount);
int readProductsFromFile(const char *filename, Product products[]);

// CART MENU
void addToCart(const char *username, int product_id, const char *product_name, int price, int quantity);
void viewCart(const char *username);

// CHECK OUT
int loadCityData(const char *filename, CityDistance city_data[]);
int getDistance(CityDistance city_data[], int num_cities, const char *city);
int calculateTotalPrice(const char *username);
void checkout(User *userActive);

#endif