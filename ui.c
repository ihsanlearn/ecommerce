#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "utils.h"
#include "seller.h"
#include "ui.h"

void displayReceipt(User *userActive, int *itemID, int itemCount, char city[], int distance) {
   FILE *cartFile = fopen("database/cart.txt", "r");
   if (!cartFile) {
      printf("Error opening cart file!\n");
      return;      
   }

   printf("====== RECEIPT ======\n");
   char line[256];
   int totalPostage = TARIF_PER_KM * distance, totalPrice = 0;

   while (fgets(line, sizeof(line), cartFile)) {
      char user[100], productName[100], productDesc[255];
      int itemIDNow, productID, quantity, price, weight;

      sscanf(line, "%[^|]|%d|%d|%d|%d|%d|%[^|]|%[^|]", user, &itemIDNow, &productID, &quantity, &price, &weight, productName, productDesc);

      if (strcmp(user, userActive->name) == 0) {
         for (int i = 0; i < itemCount; i++) {
            if (itemID[i] == itemIDNow) {
               totalPostage += TARIF_PER_KG * weight;
               totalPrice += price;

               printf("-------------------------------------\n");
               printf("Product ID     : %d\n", productID);
               printf("Product Name   : %s\n", productName);
               printf("Product Qty    : %d\n", quantity);
               printf("Product Price  : %d\n", price);
            }
         }
      }
   }
   printf("-------------------------------------\n");
   printf("Deliver to     : %s\n", city);
   printf("Postage        : %d\n", totalPostage);
   printf("Total Price    : %d\n", totalPrice);
   printf("-------------------------------------\n");
   printf("Saldo          : %d\n", userActive->saldo);
   printf("-------------------------------------\n");

   fclose(cartFile);
}