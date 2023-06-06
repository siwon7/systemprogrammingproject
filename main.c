/*
책, 도서관, 유저의 정보를 등록한다.
*/

#include "/usr/include/mysql/mysql.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


int check_barcode(MYSQL_RES* res, int barcode);
int check_loc(MYSQL_RES* res, char* location);

#define HOST NULL

int main(int argc, char** argv) {
    MYSQL mysql;
    
    mysql_init(&mysql);
    int fields;
    
    // real_connect: mysql 포인터, ip, user, passwd, db, port, null, 0

    if(!mysql_real_connect(&mysql, HOST, "root", "1234", "team_proj", 3306, (char*)NULL, 0)) {
        printf("%s\n", mysql_error(&mysql));
        exit(1);
    }
    while(true) {
        MYSQL_RES* res;
        int func_num = 0;
        printf("사용할 기능을 선택하세요. 1. 책 등록, 2. 도서관 등록, 3. 유저 등록 0. 나가기: ");
        scanf("%d", &func_num);
        getchar();

        if (func_num == 1) {
            printf("책을 등록하겠습니다.\n");
            int barcode = 0;
            srand((unsigned int)time(NULL)); // 책 바코드 생성. 이건 추후 바코드 읽는 것으로 대체가능.
            barcode = rand();
            char query[1024] = "SELECT barcode FROM book_info";
            if(mysql_query(&mysql, query)) {
                printf("%s\n", mysql_error(&mysql));
                exit(1);
            }
            res = mysql_store_result(&mysql); // 같은 바코드가 존재하는지 확인.
            if(check_barcode(res, barcode)) barcode = rand();

            int loc_id;
            char input_str[100], *s, *tmp, temp_string[101];
            int cnt = 0;
            char* book_info[4];
            printf("책의 정보를 입력해주세요. (제목/저자/분류)");
            fgets(input_str, sizeof(input_str), stdin);
            s = strtok(input_str, "/");
            while(s != NULL) {
                book_info[cnt] = s;
                s = strtok(NULL, "/");
                cnt++;
            }

            printf("도서관 id를 입력해주세요.");
            scanf("%d", &loc_id);

            query[0] = '\0';
            sprintf(query, "INSERT INTO book_info (title, author, type, loc_id, barcode) VALUES ('%s', '%s', '%s', %d, %d)",
            book_info[0], book_info[1], book_info[2], loc_id, barcode);

            if(mysql_query(&mysql, query)) {
                printf("%s\n", mysql_error(&mysql));
                exit(1);
            }
        }
        else if(func_num == 2) {
            printf("도서관 정보를 등록하겠습니다.\n");
            int loc_id = 0;
            srand((unsigned int)time(NULL));
            loc_id = rand();
            char query[1024] = "SELECT loc_id, loc FROM lib_info";
            if(mysql_query(&mysql, query)) {
                printf("%s\n", mysql_error(&mysql));
                exit(1);
            }
            res = mysql_store_result(&mysql); // 같은 바코드가 존재하는지 확인.

            char input_str[100], *s;
            printf("도서관 정보를 입력해주세요. :");
            fgets(input_str, sizeof(input_str), stdin);

            MYSQL_ROW row;
            int state = 0;
            while((row = mysql_fetch_row(res))) {
                char* id_tmp = row[0];
                char* loc_tmp = row[1];
                int tmpI = atoi(id_tmp);
                if (tmpI == loc_id) loc_id = rand();
                if (!strcmp(loc_tmp, input_str)) {
                    printf("이미 등록된 주소입니다.\n");
                    state = 1;
                    break;
                }
            }
            if(state == 1) break;

            printf("도서관 ID는 %d 입니다.\n", loc_id);
            query[0] = '\0';
            sprintf(query, "INSERT INTO lib_info (loc_id, loc) VALUES (%d, '%s')",
            loc_id, input_str);

            if(mysql_query(&mysql, query)) {
                printf("%s\n", mysql_error(&mysql));
                exit(1);
            }
        }
        else if (func_num == 3) {
            printf("사용자를 등록하겠습니다.\n");
            int user_id = 0;
            printf("학번을 입력하세요. :");
            scanf("%d", &user_id);
            getchar();
            char query[1024] = "SELECT user_id FROM user_info";
            if(mysql_query(&mysql, query)) {
                printf("%s\n", mysql_error(&mysql));
                exit(1);
            }
            res = mysql_store_result(&mysql);
            if(check_barcode(res, user_id)) {
                printf("이미 존재하는 학생입니다.\n");
                break;
            }
            char name[100];
            printf("이름을 입력해주세요. :");
            fgets(name, sizeof(name), stdin);
            
            query[0] = '\0';
            sprintf(query, "INSERT INTO user_info (user_id, name) VALUES (%d, '%s')",
            user_id, name);

            if(mysql_query(&mysql, query)) {
                printf("%s\n", mysql_error(&mysql));
                exit(1);
            }
        }
        else if (func_num == 0) {
            printf("등록을 종료합니다.\n");
            break;
        }
        else {
            printf("잘못된 입력입니다.\n");
        }
    }

    mysql_close(&mysql);
    return 0;
}

int check_barcode(MYSQL_RES* res, int barcode) {
    int fields = mysql_num_fields(res);
    MYSQL_ROW row;
    while((row = mysql_fetch_row(res))) {
        for(int i = 0;i<fields;i++) {
            char* tmp = row[i];
            int tmp_I = atoi(tmp);
            if(tmp_I == barcode) return 1;
        }
    }
    return 0;
}

int check_loc(MYSQL_RES* res, char* location) {
    int fields = mysql_num_fields(res);
    MYSQL_ROW row;
    while((row = mysql_fetch_row(res))) {
        char* tmp = row[1];
        if(row[1] == location) return 1;
    }
    return 0;
}