#ifndef UTILS_H
#define UTILS_H

#define USER_COLS 5
#define CART_COLS 2
#define PODUCTS_COLS 6
#define MAX_PRODUCTS 130
#define TARIF_PER_KM 2000
#define TARIF_PER_KG 3000
#define MAX_CITIES 10

#define USERS_FILE "database/users.txt"
#define ACTIVE_FILE "database/active.txt"
#define PRODUCTS_FILE "database/products.txt"

#define STATUS_WAITING "Waiting"
#define STATUS_PACKING "Packing"
#define STATUS_SHIPPED "Sent"
#define STATUS_DELIVERED "Delivered"

#define JNE_KILAT "JNE Kilat"
#define JNE "JNE"
#define JNT "JNT"
#define JMK "JMK"


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
   int id;
   char *name;
   char *password;
   char *email;
   char *address;
   int saldo;
} User;


typedef struct {
   int id;
   char name[100];
   char category[50];
   int price;
   int weight;
   int stock;
   int sold;
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
void removeNewline(char *str);

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

//DISPLAY PRODUCTS
void displayAllProducts();
void displayProductsByCategory();
void sortProductPrice();
void mostPurchaseProducts();

// CART MENU
void addToCart(const char *username, int product_id, const char *product_name, const char *productDesc, int price, int quantity, int weight);
int viewCart(const char *username);
bool isItemInCart(const char *username, int itemid);

// CHECK OUT
int loadCityData(const char *filename, CityDistance city_data[]);
int getDistance(CityDistance city_data[], int num_cities, const char *city);
int calculateTotalPrice(const char *username, int *itemID, int itemCount);

// TRANSACTION
int getNewTransactionId();
void saveTransaction(int transactionId, User *userActive, int distance, char *jasa);
void checkout(User *userActive);
const char* handleStatus(time_t transactionTime);
void displayAllTransaction(User *userActive);
void returnMenu(User *userActive);
void refreshTransactionStatus();
void returnItems(User *userActive);

#endif