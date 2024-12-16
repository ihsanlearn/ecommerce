#ifndef SELLER_H
#define SELLER_H

#define MAX_ITEMS 100

typedef struct {
    int itemId;
    char name[100];
    int price;
} Item;

int mainSellerPage();

#endif