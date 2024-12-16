#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "utils.h"
#include "seller.h"
#include "ui.h"

void myTransactions(User *userActive) {
   // refreshTransactionStatus();

   char *menu[] = {"Semua Transaksi", "Return ?", NULL};
   printMenu(menu);
   int userChoice = getUserChoice(2);

   switch (userChoice)  {
      case 1:
         clearScreen();
         displayAllTransaction(userActive);
         break;
      case 2:
         returnItems(userActive);

         break;
      
      default: 
         break;
   }
}

User *landingPage(Table *table) {
   char *menu[] = {"Sign Up", "Sign In", "Seller", "as Guest", NULL};
   printMenu(menu);
   int mainChoice = getUserChoice(4);

   switch (mainChoice) {
   case 1:
      inputUser(table);
      saveTableToFile(table, USERS_FILE);
      break;
   case 2:
      clearScreen();
      User *user = login(table);
      clearScreen();
      printf("Sign In successful! Welcome, %s\n", user->name);
      return user;
      break;
   case 3:
      mainSellerPage();
      break;
   case 4:
      clearScreen();
      printf("Hello, Guest...\n");
      break;
   
   default:
      break;
   }

   return NULL;
}

void profileMenu(User **userActive, Product products[]) {
Profile:
   char *menu[] = {"Cart", "Check Out", "Transaction", "Log Out", NULL};
   printMenu(menu);
   int response = getUserChoice(4);

   switch (response) {
      case 1:
         clearScreen();
         viewCart((*userActive)->name);
         break;
      case 2:
         clearScreen(); 
         checkout(*userActive);
         break;
      case 3:
         clearScreen();
         myTransactions(*userActive);
         break;
      case 4:
         logOut((*userActive)->name);
         *userActive = NULL;
         printf("You have successfully logged out.\n");
         break;
   }
}

void mainMenu(User *userActive, Table *table, Product products[]) {
Sign:
   userActive = landingPage(table);

Main:
   char *menu[] = {"Search Products", "Display Products by Category", ((userActive != NULL) ? "Add to Cart" : "Log In or Sign Up"), ((userActive != NULL) ? "Profile" : NULL), NULL};
   printMenu(menu);
   int response = getUserChoice(4);

   switch (response) {
      case 1:
         clearScreen();
         int productCount = readProductsFromFile("database/products.txt", products);
         searchProduct(products, productCount);
         goto Main;
         break;
      case 2:
         displayProductsByCategory();
         goto Main;
         break;
      case 3: {
         clearScreen();
         if (userActive != NULL) {
            int productCount = readProductsFromFile("database/products.txt", products);
            printf("Enter Product ID to add to cart: ");
            int productId = getUserChoice(MAX_PRODUCTS);

            Product *selectedProduct = NULL;
            for (int i = 0; i < productCount; i++) {
               if (products[i].id == productId) {
                  selectedProduct = &products[i];
                  break;
               }
            }

            if (selectedProduct == NULL) {
               printf("Product not found.\n");
               goto Main;
            }

            printf("Enter quantity: ");
            int quantity = getUserChoice(selectedProduct->stock);

            removeNewline(selectedProduct->description);
            addToCart(userActive->name, selectedProduct->id, selectedProduct->name, selectedProduct->description, selectedProduct->price, quantity, selectedProduct->weight);
            printf("Item successfully added to cart\n");
            goto Main;
         } else {
            goto Sign;
         }
         break;
      }
      case 4:
         clearScreen();
         profileMenu(&userActive, products);
         goto Main;
         break;
   }
}

int main() {
   Table table = {NULL, 0};
   User *userActive = NULL;
   Product products[MAX_PRODUCTS];

   // Like here
   mainMenu(userActive, &table, products);

   freeUser(userActive);
   return 0;
}