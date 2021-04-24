#include <sys/types.h>
#include <sys/ipc.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

void print_error(){
    char error_text[100];
    perror(error_text);
    printf("Error: %s\n", error_text);
}


int main(){
    int dns_socket;

    dns_socket = socket(AF_INET, SOCK_STREAM, 0);

    if(dns_socket < 0){
        printf("Can't create socket");
        print_error();
        exit(1);
    }

    struct sockaddr_in sock_info;
    sock_info.sin_family = AF_INET;
    sock_info.sin_port = htons(2222);
    sock_info.sin_addr.s_addr = htonl(INADDR_ANY);

    int dns_bind = bind(dns_socket, (struct sockaddr*) &sock_info, sizeof(sock_info));
    if(dns_bind < 0){
        printf("Bind error\n");
        print_error();
        exit(1);
    }

    int serv_work = 1;
    //Вввод dns-данных
    FILE* inp_file = fopen("dns.txt", "r");
    if(inp_file == 0){
        printf("Can't open file\n");
        exit(1);
    }

    char **text_addres;
    char **ip_addres;
    int dns_n;



    fscanf(inp_file, "%d", &dns_n);

    text_addres = (char**)malloc(sizeof(char*) * dns_n);
    ip_addres = (char**)malloc(sizeof(char*) * dns_n);

    int read_iter;
    for(read_iter = 0 ; read_iter < dns_n; read_iter++){
        text_addres[read_iter] = (char*)malloc(sizeof(char) * 100);
        ip_addres[read_iter] = (char*)malloc(sizeof(char) * 100);
        fscanf(inp_file, "%s%s", text_addres[read_iter], ip_addres[read_iter]);
    }
    fclose(inp_file);


    printf("\t!!! DNS Table!!!\n");
    for(read_iter = 0 ; read_iter < dns_n; read_iter++)
        printf("%s\t%s\n", text_addres[read_iter], ip_addres[read_iter]);

    printf("\n\n\n");

    while(serv_work){
        int check_lis = listen(dns_socket, 3);
        if(check_lis < 0){
            printf("Listen error\n");
            print_error();
            exit(1);
        }
        int dns_client = accept(dns_socket, NULL, NULL);

        char txt_addr[100];
        int recv_bytes = 0;
        while(recv_bytes < 1)
            recv_bytes = recv(dns_client, txt_addr, sizeof(txt_addr), 0);

        printf("Request:\t%s\n", txt_addr);

        if(strcmp(txt_addr, "end") == 0){
            close(dns_client);
            serv_work = 0;
        }

        if(serv_work){
            int find_iter;
            int find_flag = 0;
            for(find_iter = 0; find_iter < dns_n && !find_flag; find_iter++){
                if(strcmp(txt_addr, text_addres[find_iter]) == 0){
                    send(dns_client, ip_addres[find_iter], strlen(ip_addres[find_iter]), 0);
                    printf("Answer:\t%s\n", ip_addres[find_iter]);
                    find_flag = 1;
                }
            }

            if(find_flag == 0){
                char error_str[] = "!Not such url!";
                send(dns_client, error_str, strlen(error_str)+1, 0);
            }
            close(dns_client);
        }

        }
    close(dns_socket);

    int free_iter;
    for(free_iter = 0; free_iter < dns_n; free_iter++){
        free(text_addres[free_iter]);
        free(ip_addres[free_iter]);
    }

    free(text_addres);
    free(ip_addres);

    return 0;
}
