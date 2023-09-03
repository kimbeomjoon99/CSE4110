#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "mysql.h"
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
using namespace std;

#pragma comment(lib, "libmysql.lib")

const char* host = "localhost";
const char* user = "root";
const char* pw = "zoon5953";
const char* db = "project2";

FILE* C, * I, * D;

int main(void) {

	MYSQL* connection = NULL;
	MYSQL conn;
	MYSQL_RES* sql_result;
	MYSQL_ROW sql_row;

	if (mysql_init(&conn) == NULL)
		printf("mysql_init() error!");

	connection = mysql_real_connect(&conn, host, user, pw, db, 3306, (const char*)NULL, 0);
	if (connection == NULL)
	{
		printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
		return 1;
	}

	else
	{
		printf("Connection Succeed\n");

		if (mysql_select_db(&conn, db))
		{
			printf("%d ERROR : %s\n", mysql_errno(&conn), mysql_error(&conn));
			return 1;
		}

		C = fopen("20180492_create.txt", "r");
		I = fopen("20180492_insert.txt", "r");
		D = fopen("20180492_delete.txt", "r");
		if (!C || !I || !D) {
			printf("File Open ERROR.\n");
			return 1;
		}

		char text_sql[10000];
		while (!feof(C)) {
			fgets(text_sql, 10000, C);
			mysql_query(connection, text_sql);
		}
		while (!feof(I)) {
			fgets(text_sql, 10000, I);
			mysql_query(connection, text_sql);
		}
		
		while (1) {
			printf("------- SELECT QUERY TYPES -------\n\n");
			printf("\t1. TYPE I\n");
			printf("\t2. TYPE II\n");
			printf("\t3. TYPE III\n");
			printf("\t4. TYPE IV\n");
			printf("\t5. TYPE V\n");
			printf("\t0. QUIT\n");
			printf("Select query type : ");
			int command;
			scanf("%d", &command);
			if (!command) {
				printf("\nQuit from DBMS engine.\n");
				break;
			}
			else if (command == 1) {
				printf("----- Subtypes in TYPE I -----\n");
				printf("\t1. TYPE I-1.\n");
				printf("\t2. TYPE I-2.\n");
				printf("\t3. TYPE I-3.\n");
				printf("Select query type of TYPE I : ");
				int subcommand;
				scanf("%d", &subcommand);
				if (subcommand == 1) {
					printf("---- TYPE I-1 ----\n\n");
					printf("**Find all customers who had a package on the truck at the time of the crash**\n");
					vector<string> cid;
					string query = "select distinct P.customer_ID from package P natural join tracking T where T.shipping_key = 'truck_1721' and T.trackdate = '2023-03-03' and P.package_ID = T.package_ID order by P.customer_ID";
					int state = 0;
					state = mysql_query(connection, query.c_str());
					if (state == 0)
					{
						sql_result = mysql_store_result(connection);
						while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
						{
							string tmp(sql_row[0]);
							cid.push_back(tmp);
						}
						mysql_free_result(sql_result);
					}
					if (!cid.size()) {
						printf("| none. |\n\n");
						continue;
					}
					for (int i = 0; i < cid.size(); i++) {
						string subquery = "select customer_ID, name from customer where customer_ID = '" + cid[i] + "'";
						state = 0;
						state = mysql_query(connection, subquery.c_str());
						if (state == 0)
						{
							sql_result = mysql_store_result(connection);
							while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
							{
								printf("| %s %s ", sql_row[0], sql_row[1]);
							}
							
							mysql_free_result(sql_result);
							
						}
					}
					printf("|\n\n");
				}
				else if (subcommand == 2) {
					printf("---- TYPE I-2 ----\n\n");
					printf("**Find all recipients who had a package on the truck at the time of the crash**\n");
					string query = "select recipient from package natural join tracking where shipping_key = 'truck_1721' and trackdate = '2023-03-03'";
					int state = 0;
					state = mysql_query(connection, query.c_str());
					if (state == 0)
					{
						int flag = 0;
						sql_result = mysql_store_result(connection);
						while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
						{
							if (!flag) flag++;
							printf("| %s ", sql_row[0]);
						}
						if (!flag) printf("| none. ");
						mysql_free_result(sql_result);
					}
					printf("|\n\n");
				}
				else if (subcommand == 3) {
					printf("---- TYPE I-3 ----\n\n");
					printf("**Find the last successful delivery by that truck prior to the crash**\n");
					string query = "select shipment_ID, trackdate from tracking where shipping_key = 'truck_1721' and trackdate < '2023-03-03' order by trackdate desc limit 1";
					int state = 0;
					state = mysql_query(connection, query.c_str());
					if (state == 0)
					{
						int flag = 0;
						sql_result = mysql_store_result(connection);
						while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
						{
							if (!flag) flag++;
							printf("Shipment_ID of last successful delivery process : %s at %s ", sql_row[0], sql_row[1]);
						}
						if (!flag) printf("none. ");
						mysql_free_result(sql_result);
					}
					printf("\n\n");
				}
				else if (!subcommand) {
					printf("\n");
					continue;
				}
			}
			else if (command == 2) {
				printf("---- TYPE II ----\n\n");
				printf("**Find the customer who has shipped the most packages in the past year**\n");
				string query = "select customer_ID, name, cummulative_shipment from customer where cummulative_shipment = (select max(cummulative_shipment) from customer)";
				int state = 0;
				state = mysql_query(connection, query.c_str());
				if (state == 0)
				{
					sql_result = mysql_store_result(connection);
					while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
					{
						printf("| %s %s ; cummulative_shipment : %s ", sql_row[0], sql_row[1], sql_row[2]);
					}
					mysql_free_result(sql_result);
				}
				printf("|\n\n");
			}
			else if (command == 3) {
				printf("---- TYPE III ----\n\n");
				printf("**Find the customer who has spent the most money on shipping in the past year**\n");
				string query = "select customer_ID, name, cummulative_cost from customer where cummulative_cost = (select max(cummulative_cost) from customer)";
				int state = 0;
				state = mysql_query(connection, query.c_str());
				if (state == 0)
				{
					sql_result = mysql_store_result(connection);
					while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
					{
						printf("| %s %s ; cummulative_cost : %s ", sql_row[0], sql_row[1], sql_row[2]);
					}
					mysql_free_result(sql_result);
				}
				printf("|\n\n");
			}
			else if (command == 4) {
				printf("---- TYPE IV ----\n\n");
				printf("**Find the packages that were not delivered withen the promised time**\n");
				string query = "select distinct P.package_ID, P.customer_ID from package P natural join tracking T natural join shipment S where P.package_ID = T.package_ID and T.shipment_ID = S.shipment_ID and T.shipping_key = 'complete' and T.trackdate > S.prearranged_time";
				int state = 0;
				state = mysql_query(connection, query.c_str());
				if (state == 0)
				{
					int flag = 0;
					sql_result = mysql_store_result(connection);
					while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
					{
						if (!flag) flag++;
						printf("| package_ID : %s, customer_ID : %s ", sql_row[0], sql_row[1]);
					}
					if (!flag) printf("| none. ");
					mysql_free_result(sql_result);
				}
				printf("|\n\n");
			}
			else if (command == 5) {
				printf("---- TYPE V ----\n\n");
				printf("**Generate the bill for each customer for the past month**\n");
				printf(" Which Customer? (Customer ID) : ");
				string customer_ID;
				int bill_type;
				cin >> customer_ID;
				printf(" Which type? (type 1: simple, type 2: by type of service, type 3: by each shipment) : ");
				scanf("%d", &bill_type);
				string st_date = "2023-03-01";
				string ed_date = "2023-03-31";
				// title
				string query = "select name from customer where customer_ID = '" + customer_ID + "'";
				string name;
				int state = 0;
				state = mysql_query(connection, query.c_str());
				if (state == 0)
				{
					int flag = 0;
					sql_result = mysql_store_result(connection);
					while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
					{
						if (!flag) flag++;
						name = string(sql_row[0]);
					}
					mysql_free_result(sql_result);
					if (!flag) {
						printf("There is no customer who has customer ID : %s\n\n", customer_ID.c_str());
						continue;
					}
				}
				printf("\n\t\t%s %s bill\n\n", customer_ID.c_str(), name.c_str());
				// simple bill
				if (bill_type == 1) {
					query.clear();
					query = "select distinct address, unpaid_cost from payment where customer_ID = '" + customer_ID + "' and payment_date >= '2023-03-01' and payment_date <= '2023-03-31'";
					state = 0;
					state = mysql_query(connection, query.c_str());
					if (state == 0)
					{
						int flag = 0;
						sql_result = mysql_store_result(connection);
						while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
						{
							if (!flag) flag++;
							printf("| address : %50s ; unpaid cost : %6s |\n", sql_row[0], sql_row[1]);
						}
						mysql_free_result(sql_result);
						if (!flag) {
							printf("There is no payment for this customer.\n\n", customer_ID.c_str());
							continue;
						}
					}
					printf("\n\n");
				}
				// by type of service
				if (bill_type == 2) {
					// payment id
					query.clear();
					query = "select payment_ID from payment where customer_ID = '" + customer_ID + "' and payment_date >= '" + st_date + "' and payment_date <= '" + ed_date + "' order by payment_ID";
					vector<string> pay_ID;
					state = 0;
					state = mysql_query(connection, query.c_str());
					if (state == 0)
					{
						int flag = 0;
						sql_result = mysql_store_result(connection);
						while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
						{
							if (!flag) flag++;
							string tmp(sql_row[0]);
							pay_ID.push_back(tmp);
						}
						mysql_free_result(sql_result);
						if (!flag) {
							printf("There is no payment for this customer.\n\n", customer_ID.c_str());
							continue;
						}
					}
					printf("|");
					for (int i = 0; i < pay_ID.size(); i++) {
						printf(" payment ID : %12s \t |", pay_ID[i].c_str());
					}
					printf("\n");
					// total cost
					query.clear();
					query = "select total_cost from payment where customer_ID = '" + customer_ID + "' and payment_date >= '" + st_date + "' and payment_date <= '" + ed_date + "' order by payment_ID";
					vector<string> tot_cost;
					state = 0;
					state = mysql_query(connection, query.c_str());
					if (state == 0)
					{
						sql_result = mysql_store_result(connection);
						while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
						{
							string tmp(sql_row[0]);
							tot_cost.push_back(tmp);
						}
						mysql_free_result(sql_result);
					}
					printf("|");
					for (int i = 0; i < tot_cost.size(); i++) {
						printf(" total cost : %12s \t |", tot_cost[i].c_str());
					}
					printf("\n");
					// payment type
					query.clear();
					query = "select payment_type from payment where customer_ID = '" + customer_ID + "' and payment_date >= '" + st_date + "' and payment_date <= '" + ed_date + "' order by payment_ID";
					vector<string> paytype;
					state = 0;
					state = mysql_query(connection, query.c_str());
					if (state == 0)
					{
						sql_result = mysql_store_result(connection);
						while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
						{
							string tmp(sql_row[0]);
							paytype.push_back(tmp);
						}
						mysql_free_result(sql_result);
					}
					printf("|");
					for (int i = 0; i < paytype.size(); i++) {
						if (!strcmp(paytype[i].c_str(), "credit_card")) printf(" payment type : %10s \t |", paytype[i].c_str());
						else if (!strcmp(paytype[i].c_str(), "account_transfer")) printf(" payment type : %s|", paytype[i].c_str());
					}
					printf("\n");
					// payment date
					query.clear();
					query = "select payment_date from payment where customer_ID = '" + customer_ID + "' and payment_date >= '" + st_date + "' and payment_date <= '" + ed_date + "' order by payment_ID";
					vector<string> paydate;
					state = 0;
					state = mysql_query(connection, query.c_str());
					if (state == 0)
					{
						sql_result = mysql_store_result(connection);
						while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
						{
							string tmp(sql_row[0]);
							paydate.push_back(tmp);
						}
						mysql_free_result(sql_result);
					}
					printf("|");
					for (int i = 0; i < paydate.size(); i++) {
						printf(" payment date : %10s \t |", paydate[i].c_str());
					}
					printf("\n");
					// unpaid cost
					query.clear();
					query = "select unpaid_cost from payment where customer_ID = '" + customer_ID + "' and payment_date >= '" + st_date + "' and payment_date <= '" + ed_date + "' order by payment_ID";
					vector<string> unpaid;
					state = 0;
					state = mysql_query(connection, query.c_str());
					if (state == 0)
					{
						sql_result = mysql_store_result(connection);
						while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
						{
							string tmp(sql_row[0]);
							unpaid.push_back(tmp);
						}
						mysql_free_result(sql_result);
					}
					printf("|");
					for (int i = 0; i < unpaid.size(); i++) {
						printf(" unpaid cost : %11s \t |", unpaid[i].c_str());
					}
					printf("\n");
					// package id
					query.clear();
					query = "select package_ID from payment where customer_ID = '" + customer_ID + "' and payment_date >= '" + st_date + "' and payment_date <= '" + ed_date + "' order by payment_ID";
					vector<string> pack_id;
					state = 0;
					state = mysql_query(connection, query.c_str());
					if (state == 0)
					{
						sql_result = mysql_store_result(connection);
						while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
						{
							string tmp(sql_row[0]);
							pack_id.push_back(tmp);
						}
						mysql_free_result(sql_result);
					}
					printf("|");
					for (int i = 0; i < pack_id.size(); i++) {
						printf(" package id : %12s \t |", pack_id[i].c_str());
					}
					printf("\n");
					// shipper id
					printf("|");
					vector<string> shipper;
					int flag = 0;
					for (int i = 0; i < pay_ID.size(); i++) {
						query.clear();
						query = "select shipper_ID from payment_through_shipper where payment_ID = '" + pay_ID[i] + "'";
						string sid;
						state = 0;
						state = mysql_query(connection, query.c_str());
						if (state == 0)
						{
							sql_result = mysql_store_result(connection);
							while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
							{
								if (!flag) flag++;
								string tmp(sql_row[0]);
								sid = tmp;
								shipper.push_back(tmp);
							}
							mysql_free_result(sql_result);
							if (!flag) shipper.push_back("none");
						}
						if (flag) printf(" shipper_ID : %12s\t |", sid.c_str());
						else printf(" did not pay by shipper    \t |");
					}
					printf("\n");
					// shipper name
					printf("|");
					flag = 0;
					for (int i = 0; i < shipper.size(); i++) {
						if (shipper[i] == "none") {
							printf(" did not pay by shipper    \t |");
							continue;
						}
						query.clear();
						query = "select name from shipper where shipper_ID = '" + shipper[i] + "'";
						string sname;
						state = 0;
						state = mysql_query(connection, query.c_str());
						if (state == 0)
						{
							sql_result = mysql_store_result(connection);
							while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
							{
								if (!flag) flag++;
								string tmp(sql_row[0]);
								sname = tmp;
							}
							mysql_free_result(sql_result);
						}
						if (flag) printf(" shipper name : %10s \t |", sname.c_str());
					}
					printf("\n");
					// customer account info
					query.clear();
					query = "select account from customer where customer_ID = '" + customer_ID + "'";
					state = 0;
					state = mysql_query(connection, query.c_str());
					if (state == 0)
					{
						sql_result = mysql_store_result(connection);
						while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
						{
							printf("***** customer's account number : %s *****\n", sql_row[0]);
						}
						mysql_free_result(sql_result);
					}
					printf("\n\n");
				}
				else if (bill_type == 3) {
					// payment id
					query.clear();
					query = "select payment_ID from payment where customer_ID = '" + customer_ID + "' and payment_date >= '" + st_date + "' and payment_date <= '" + ed_date + "' order by payment_ID";
					vector<string> pay_ID;
					state = 0;
					state = mysql_query(connection, query.c_str());
					if (state == 0)
					{
						int flag = 0;
						sql_result = mysql_store_result(connection);
						while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
						{
							if (!flag) flag++;
							string tmp(sql_row[0]);
							pay_ID.push_back(tmp);
						}
						mysql_free_result(sql_result);
						if (!flag) {
							printf("There is no payment for this customer.\n\n", customer_ID.c_str());
							continue;
						}
					}
					printf("|");
					for (int i = 0; i < pay_ID.size(); i++) {
						printf(" payment ID : %12s \t |", pay_ID[i].c_str());
					}
					printf("\n");
					// total cost
					query.clear();
					query = "select total_cost from payment where customer_ID = '" + customer_ID + "' and payment_date >= '" + st_date + "' and payment_date <= '" + ed_date + "' order by payment_ID";
					vector<string> tot_cost;
					state = 0;
					state = mysql_query(connection, query.c_str());
					if (state == 0)
					{
						sql_result = mysql_store_result(connection);
						while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
						{
							string tmp(sql_row[0]);
							tot_cost.push_back(tmp);
						}
						mysql_free_result(sql_result);
					}
					printf("|");
					for (int i = 0; i < tot_cost.size(); i++) {
						printf(" total cost : %12s \t |", tot_cost[i].c_str());
					}
					printf("\n");
					// payment type
					query.clear();
					query = "select payment_type from payment where customer_ID = '" + customer_ID + "' and payment_date >= '" + st_date + "' and payment_date <= '" + ed_date + "' order by payment_ID";
					vector<string> paytype;
					state = 0;
					state = mysql_query(connection, query.c_str());
					if (state == 0)
					{
						sql_result = mysql_store_result(connection);
						while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
						{
							string tmp(sql_row[0]);
							paytype.push_back(tmp);
						}
						mysql_free_result(sql_result);
					}
					printf("|");
					for (int i = 0; i < paytype.size(); i++) {
						if (!strcmp(paytype[i].c_str(), "credit_card")) printf(" payment type : %10s \t |", paytype[i].c_str());
						else if (!strcmp(paytype[i].c_str(), "account_transfer")) printf(" payment type : %s|", paytype[i].c_str());
					}
					printf("\n");
					// payment date
					query.clear();
					query = "select payment_date from payment where customer_ID = '" + customer_ID + "' and payment_date >= '" + st_date + "' and payment_date <= '" + ed_date + "' order by payment_ID";
					vector<string> paydate;
					state = 0;
					state = mysql_query(connection, query.c_str());
					if (state == 0)
					{
						sql_result = mysql_store_result(connection);
						while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
						{
							string tmp(sql_row[0]);
							paydate.push_back(tmp);
						}
						mysql_free_result(sql_result);
					}
					printf("|");
					for (int i = 0; i < paydate.size(); i++) {
						printf(" payment date : %10s \t |", paydate[i].c_str());
					}
					printf("\n");
					// unpaid cost
					query.clear();
					query = "select unpaid_cost from payment where customer_ID = '" + customer_ID + "' and payment_date >= '" + st_date + "' and payment_date <= '" + ed_date + "' order by payment_ID";
					vector<string> unpaid;
					state = 0;
					state = mysql_query(connection, query.c_str());
					if (state == 0)
					{
						sql_result = mysql_store_result(connection);
						while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
						{
							string tmp(sql_row[0]);
							unpaid.push_back(tmp);
						}
						mysql_free_result(sql_result);
					}
					printf("|");
					for (int i = 0; i < unpaid.size(); i++) {
						printf(" unpaid cost : %11s \t |", unpaid[i].c_str());
					}
					printf("\n");
					// package id
					query.clear();
					query = "select package_ID from payment where customer_ID = '" + customer_ID + "' and payment_date >= '" + st_date + "' and payment_date <= '" + ed_date + "' order by payment_ID";
					vector<string> pack_id;
					state = 0;
					state = mysql_query(connection, query.c_str());
					if (state == 0)
					{
						sql_result = mysql_store_result(connection);
						while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
						{
							string tmp(sql_row[0]);
							pack_id.push_back(tmp);
						}
						mysql_free_result(sql_result);
					}
					printf("|");
					for (int i = 0; i < pack_id.size(); i++) {
						printf(" package id : %12s \t |", pack_id[i].c_str());
					}
					printf("\n");
					// shipment_ID
					printf("|");
					for (int i = 0; i < pack_id.size(); i++) {
						query.clear();
						query = "select distinct shipment_ID from tracking where package_ID = '" + pack_id[i] + "'";
						state = 0;
						state = mysql_query(connection, query.c_str());
						if (state == 0)
						{
							sql_result = mysql_store_result(connection);
							while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
							{
								printf(" shipment id : %11s \t |", sql_row[0]);
							}
							mysql_free_result(sql_result);
						}
					}
					printf("\n");
					// content
					printf("|");
					for (int i = 0; i < pack_id.size(); i++) {
						query.clear();
						query = "select content from package where package_ID = '" + pack_id[i] + "'";
						state = 0;
						state = mysql_query(connection, query.c_str());
						if (state == 0)
						{
							sql_result = mysql_store_result(connection);
							while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
							{
								printf(" content : %15s \t |", sql_row[0]);
							}
							mysql_free_result(sql_result);
						}
					}
					printf("\n");
					// value
					printf("|");
					for (int i = 0; i < pack_id.size(); i++) {
						query.clear();
						query = "select value from package where package_ID = '" + pack_id[i] + "'";
						state = 0;
						state = mysql_query(connection, query.c_str());
						if (state == 0)
						{
							sql_result = mysql_store_result(connection);
							while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
							{
								printf(" value : %17s \t |", sql_row[0]);
							}
							mysql_free_result(sql_result);
						}
					}
					printf("\n\n");
				}
			}
			else printf("Choose 0-5 query type.\n\n");
		}
		
		while (!feof(D)) {
			fgets(text_sql, 10000, D);
			mysql_query(connection, text_sql);
		}
		
		fclose(C);
		fclose(I);
		fclose(D);

		mysql_close(connection);
	}

	return 0;
}
