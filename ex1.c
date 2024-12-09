#include <stdio.h>
#include <unistd.h>  //Used for UART
#include <fcntl.h>   //Used for UART
#include <termios.h> //Used for UART
#include <stdlib.h>
#include <string.h>

void solicitacao_dados(void);

int main()
{
	solicitacao_dados();
    return 0;
}

void solicitacao_dados(void){
	    int uart0_filestream = -1;

    uart0_filestream = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY); // Open in non blocking read/write mode
    if (uart0_filestream == -1)
    {
        printf("Erro - Não foi possível iniciar a UART.\n");
    }
    struct termios options;
    tcgetattr(uart0_filestream, &options);
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD; //<Set baud rate
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart0_filestream, TCIFLUSH);
    tcsetattr(uart0_filestream, TCSANOW, &options);

    unsigned char tx_buffer[255];
    unsigned char *p_tx_buffer;

    p_tx_buffer = &tx_buffer[0];

    *p_tx_buffer++ = 0xa1;
    *p_tx_buffer++ = 1; // 1 
    *p_tx_buffer++ = 4; // 4
    *p_tx_buffer++ = 7; // 7
    *p_tx_buffer++ = 2; // 2

    if (uart0_filestream != -1)
    {
        int count = write(uart0_filestream, &tx_buffer, (p_tx_buffer - &tx_buffer[0]));
        if (count <= 0)
        {
            printf("UART TX error\n");
        }
    }

    sleep(1);

    //----- CHECK FOR ANY RX BYTES -----
    if (uart0_filestream != -1)
    {
        // Read up to 255 characters from the port if they are there
        int rx_buffer;
        int rx_length = read(uart0_filestream, &rx_buffer, 5); // Filestream, buffer to store in, number of bytes to read (max)
        if (rx_length < 0)
        {
            printf("Erro na leitura.\n"); // An error occured (will occur if there are no bytes)
        }
        else if (rx_length == 0)
        {
            printf("Nenhum dado disponível.\n"); // No data waiting
        }
        else
        {
            // Bytes received
            printf("Solicita INT\t(0xA1) - Retorno = %d\n", rx_buffer);
        }
    }

    close(uart0_filestream);
}

/*
===============
OUTPUT
Solicita INT    (0xA1) - Retorno = 41987
Solicita FLOAT  (0xA2) - Retorno = 3.141516
Solicita String (0xA3) - Retorno = "Mensagem de Teste pela UART"

Envia INT   (0xB1 + valor_int)   - Retorno = valor_int x 2
Envia FLOAT (0xB2 + valor_float) - Retorno = valor_float x 2
Envia String(0xB3 + string_enviada) - Retorno = "String Recebida:" + string_enviada
=================
USEFUL LINKS
https://gitlab.com/fse_fga/raspberry-pi/exercicios/exercicio-1-uart
https://youtube.com/live/OSB10KSw3zw?feature=share
https://gitlab.com/fse_fga/raspberry-pi/uart_raspberry_pi/-/tree/master/c/src?ref_type=heads
===================
ssh rodolfoneves@164.41.98.26  -p 13508


*/