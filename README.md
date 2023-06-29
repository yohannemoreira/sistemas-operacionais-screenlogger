# ScreenLogger - Disciplina de Sistemas Operacionais I
Projeto da Disciplina de Sistemas Operacionais. O programa consiste em um screenlogger que irá captura imagens da tela da máquina infectada e enviar via sockets para outra máquina da mesma rede
## - Sobre o Servidor:
Este código é um exemplo básico de um servidor TCP em C que recebe imagens de clientes e as salva em um diretório específico.

1. Inclusão de bibliotecas:
   - `stdio.h`: Biblioteca de entrada/saída padrão.
   - `stdlib.h`: Biblioteca padrão do C que contém funções como `exit()` para encerrar o programa.
   - `unistd.h`: Biblioteca para funcionalidades relacionadas ao sistema operacional, como a função `read()`.
   - `string.h`: Biblioteca para manipulação de strings.
   - `arpa/inet.h`: Biblioteca para funcionalidades relacionadas à internet, como estruturas de endereço IP.
   - `time.h`: Biblioteca para trabalhar com funções relacionadas ao tempo.

2. Constante e função de erro:
   - `MAX_BUFFER_SIZE`: Define o tamanho máximo do buffer utilizado para receber dados.
   - `error()`: Função personalizada para exibir uma mensagem de erro com `perror()` e encerrar o programa com `exit()`.

3. Função `main()`:
   - Declaração de variáveis locais:
     - `serverSocket` e `clientSocket`: Sockets do servidor e cliente, respectivamente.
     - `serverAddress` e `clientAddress`: Estruturas `sockaddr_in` para armazenar os endereços do servidor e cliente.
     - `serverPort`: Número da porta em que o servidor irá escutar.
     - `savePath`: Caminho para salvar as imagens recebidas.
   - Criação do socket do servidor:
     - `socket()`: Cria um socket utilizando o protocolo TCP (`SOCK_STREAM`).
   - Configuração do endereço do servidor:
     - `memset()`: Limpa a estrutura do endereço do servidor.
     - `AF_INET`: Família de endereços IP para IPv4.
     - `INADDR_ANY`: Endereço IP do servidor, definido como "qualquer" (`0.0.0.0`).
     - `htons()`: Converte o número da porta para o formato de rede (byte order).
   - Vinculação do socket à porta do servidor:
     - `bind()`: Associa o socket a um endereço e porta específicos.
   - Habilitação do servidor para aguardar conexões:
     - `listen()`: Coloca o socket do servidor em um estado de escuta por conexões.
   - Loop principal para receber conexões e imagens:
     - `accept()`: Aceita uma conexão de um cliente e retorna um novo socket para comunicação com esse cliente.
     - Recebimento e salvamento da imagem:
       - `read()`: Lê dados do socket do cliente para um buffer.
       - `fwrite()`: Escreve os dados do buffer em um arquivo.
       - `fclose()`: Fecha o arquivo.
       - `close()`: Fecha o socket do cliente.
   - Encerramento do socket do servidor:
     - `close()`: Fecha o socket do servidor.
## - Sobre o Cliente:
Este código captura uma screenshot da tela usando a biblioteca X11, salva a imagem em um arquivo PNG e envia o arquivo para um servidor remoto via socket. O processo é repetido continuamente, permitindo capturar e enviar várias imagens ao servidor.
1. Inclusão de bibliotecas:
   - `stdio.h`: Biblioteca de entrada/saída padrão.
   - `stdlib.h`: Biblioteca padrão do C que contém funções como `exit()` e `malloc()`.
   - `X11/Xlib.h` e `X11/Xutil.h`: Bibliotecas X11 para manipulação de janelas e captura de tela.
   - `png.h`: Biblioteca para manipulação de arquivos PNG.
   - `string.h`: Biblioteca para manipulação de strings.
   - `unistd.h`: Biblioteca para funcionalidades relacionadas ao sistema operacional, como a função `fork()`.
   - `arpa/inet.h` e `sys/socket.h`: Bibliotecas para funcionalidades de rede, como sockets.

2. Constante e função de erro:
   - `MAX_BUFFER_SIZE`: Define o tamanho máximo do buffer utilizado para ler e enviar dados via socket.
   - `error()`: Função personalizada para exibir uma mensagem de erro com `perror()` e encerrar o programa com `exit()`.

3. Função `saveScreenshot()`:
   - Abre um arquivo para escrita com `fopen()`.
   - Cria a estrutura `png_structp` para escrever o arquivo PNG.
   - Cria a estrutura `png_infop` para as informações do arquivo PNG.
   - Configura um ponto de retorno para tratamento de erros com `setjmp()`.
   - Inicializa a escrita do arquivo PNG com `png_init_io()` e configura os parâmetros do cabeçalho com `png_set_IHDR()`.
   - Escreve as informações e os dados da imagem no arquivo com `png_write_image()` e `png_write_end()`.
   - Libera a memória alocada e fecha o arquivo.

4. Função `main()`:
   - Declaração de variáveis locais:
     - `clientSocket`: Socket do cliente para enviar a imagem.
     - `serverAddress`: Estrutura `sockaddr_in` para armazenar o endereço do servidor.
     - `serverIP`: Endereço IP do servidor remoto.
     - `serverPort`: Número da porta do servidor remoto.
     - `imagePath`: Caminho do arquivo de imagem capturada.
   - Utilização do `fork()` para criar um processo filho que irá executar o código.
   - No processo pai, o programa é encerrado.
   - No processo filho:
     - Alteração da umask com `umask()` para permitir permissões de leitura e escrita nos arquivos criados.
     - Criação de uma nova sessão com `setsid()`.
     - Loop infinito para capturar e enviar a imagem continuamente.
     - Captura da imagem da tela utilizando a biblioteca X11:
       - Abertura do display com `XOpenDisplay()`.
       - Obtenção do identificador da janela raiz com `DefaultRootWindow()`.
       - Obtenção dos atributos da janela raiz com `XGetWindowAttributes()`.
       - Captura da imagem da tela com `XGetImage()`.
       - Salvamento da imagem em um arquivo com `saveScreenshot()`.
       - Liberação da imagem e fechamento do display.
     - Abertura do arquivo de imagem com `fopen()`.


     - Criação do socket do cliente com `socket()`.
     - Configuração do endereço do servidor com `memset()` e `inet_pton()`.
     - Loop para tentar conectar ao servidor repetidamente até obter sucesso.
     - Leitura e envio do arquivo da imagem via socket em partes com `fread()` e `write()`.
     - Fechamento do arquivo e do socket.
