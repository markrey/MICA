#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mosquitto.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

char *topic   = NULL;
/* debug mode flag */
int is_debug = FALSE;

/**
 * Brokerとの接続成功時に実行されるcallback関数
 */
void on_connect(struct mosquitto *mosq, void *obj, int result)
{
    if(is_debug) {
        printf("%s(%d)\n", __FUNCTION__, __LINE__);
    }
    mosquitto_subscribe(mosq, NULL, topic, 0);
}

/**
 * Brokerとの接続を切断した時に実行されるcallback関数
 */
void on_disconnect(struct mosquitto *mosq, void *obj, int rc)
{
    if(is_debug) {
        printf("%s(%d)\n", __FUNCTION__, __LINE__);
    }
}

/**
 * メッセージ受信処理
 */
void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
    if(is_debug) {
        printf("%s(%d)\n", __FUNCTION__, __LINE__);
    }

    if(message->payloadlen){
        printf("%s ", message->topic);
        fwrite(message->payload, 1, message->payloadlen, stdout);
        printf("\n");
    }else{
        printf("%s (null)\n", message->topic);
    }
    fflush(stdout);
}

/**
 * コマンド引数エラー表示関数
 */
void usage()
{
    printf("mqtt_pub -t <topic> [-d]\n");
    printf("  Required\n");
    printf("    -t topic\n");
    printf("  SSL Option\n");
    printf("    -C : CA cert file\n");
    printf("    -c : client cert file\n");
    printf("    -k : client private file\n");
    printf("  Optional\n");
    printf("    -p : port number(no ssl:1883:default, ssl:8883)\n");
    printf("    -d : debug mode\n");
    exit(EXIT_FAILURE);
}

/**
 * mqtt_subメイン関数
 */
int main(int argc, char *argv[])
{
    int   ret           = 0;
    int   cmdopt        = 0;
    char *id            = "mqtt/sub";
    char *host          = "192.168.10.55";
    int   port          = 1883;
    char *cafile        = NULL;
    char *certfile      = NULL;
    char *keyfile       = NULL;
    int   keepalive     = 60;
    bool  clean_session = true;
    struct mosquitto *mosq = NULL;

    while((cmdopt=getopt(argc, argv, "h:p:C:c:k:t:m:d")) > 0) {
        switch(cmdopt) {
        case 'h':               /* broker uri */
            host = (char*)strdup(optarg);
            break;
        case 'p':               /* port number */
            port = atoi(optarg);
            break;
        case 'C':               /* cafile cert file */
            cafile = (char*)strdup(optarg);
            break;
        case 'c':               /* client cert file */
            certfile = (char*)strdup(optarg);
            break;
        case 'k':               /* client key file */
            keyfile = (char*)strdup(optarg);
            break;
        case 't':               /* Topic */
            topic = (char*)strdup(optarg);
            break;
        case 'd':               /* debug mode */
            is_debug = TRUE;
            break;
        default:
            usage();
        }
    }
    /* topicが指定されていない場合、引数NG */
    if(topic == NULL) {
        usage();
    }
    /* クライアント証明書とクライアント秘密鍵はどちらか一方を
     * 指定した場合は、他方の指定も必須
     */
    if(
       ((certfile == NULL) && (keyfile != NULL)) ||
       ((certfile != NULL) && (keyfile == NULL))
    ) {
        usage();
    }

    if(is_debug) {
        printf("  %s\n", host);
        printf("  %d\n", port);
        printf("  %s\n", cafile);
        printf("  %s\n", certfile);
        printf("  %s\n", keyfile);
        printf("  %s\n", topic);
    }

    mosquitto_lib_init();
    mosq = mosquitto_new(id, clean_session, NULL);
    if(!mosq){
        fprintf(stderr, "Cannot create mosquitto object\n");
        mosquitto_lib_cleanup();
        return(EXIT_FAILURE);
    }
    mosquitto_connect_callback_set(mosq, on_connect);
    mosquitto_disconnect_callback_set(mosq, on_disconnect);
    mosquitto_message_callback_set(mosq, on_message);

    if(cafile != NULL) {
        ret = mosquitto_tls_set(mosq, cafile, NULL, certfile, keyfile, NULL);
        if(ret != MOSQ_ERR_SUCCESS) {
            printf("mosquitto_tls_set function is failed.\n");
        }
        mosquitto_tls_insecure_set(mosq, true);
    }

    if(mosquitto_connect_bind(mosq, host, port, keepalive, NULL)){
        fprintf(stderr, "failed to connect broker.\n");
        mosquitto_lib_cleanup();
        return(EXIT_FAILURE);
    }

    ret = mosquitto_loop_forever(mosq, -1, 1);

    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return(EXIT_SUCCESS);
}
//$ gcc -Wall -g mqtt_sub.c -o mqtt_sub -lmosquitto
