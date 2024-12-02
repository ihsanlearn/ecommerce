#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "utils.h"

User *landingPage(Table *table) {
   char *menu[] = {"Sign Up", "Sign In", "as Guest", NULL};
   printMenu(menu);
   int mainChoice = getUserChoice(3);

   if (mainChoice == 1) {
      char *menuRole[] = {"Seller", "Buyer", NULL};
      printMenu(menuRole);
      int roleChoice = getUserChoice(2);

      if (roleChoice == 2) {
         inputUser(table);
         saveTableToFile(table, USERS_FILE);
         printf("Sign Up successful!\n");
      } else { 
         printf("Seller sign up menu\n");
      }
   } else if (mainChoice == 2) {
      char *menuRole[] = {"Seller", "Buyer", NULL};
      printMenu(menuRole);
      int roleChoice = getUserChoice(2);

      if (roleChoice == 2) {
         User *user = login(table);
         printf("Sign In successful! Welcome, %s\n", user->name);
         return user;
      } else {
         printf("Seller sign in menu\n");
      }
   } else {
      printf("Proceeding as Guest...\n");
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
         goto Profile;
         break;
      case 2:
         checkout(*userActive);
         break;
      case 3:
         clearScreen();
         printf("Transaction feature is under construction.\n");
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
   clearScreen();

Main:
   printf("%s", (userActive == NULL ? "User: NULL\n" : "User: active\n"));
   char *menu[] = {"Search Products", "Display All Products", ((userActive != NULL) ? "Add to Cart" : "Log In or Sign Up"), ((userActive != NULL) ? "Profile" : NULL), NULL};
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
         // clearScreen();
         // displayAllProducts(products, MAX_PRODUCTS);
         // goto Main;
         // break;
      case 3: {
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

            addToCart(userActive->name, selectedProduct->id, selectedProduct->name, selectedProduct->price, quantity);
            goto Main;
         } else {
            clearScreen();
            goto Sign;
         }
         break;
      }
      case 4:
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