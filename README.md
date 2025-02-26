# Resumo do projeto

O projeto implementa a comunicação serial na placa BitDogLab, juntamente com manipulação de matriz de leds, led RGB individual, display SSD1306,
e uso de botões que utilizam de interrupções, pull-up e debounce para garantir a leitura correta dos cliques.

# Instruções de uso

Para uso do software, siga os seguintes passos:

- **1°:** clone o repositório para o seu computador.

    - Ao abrir o projeto com o **VSCode**, a extensão do **CMake** irá criar a pasta ``build`` para você com os arquivos de compilação.

    - Caso não seja gerada a pasta, crie uma pasta com nome `build` e execute o seguinte comando dentro da pasta criada:
        
        ``cmake ..``

        O comando acima irá criar os arquivos de compilação.

- **2°:** execute a compilação do firmware usando a extensão do ***Raspberry Pi Pico*** do ***VSCode***.

A partir daqui, seu firmware já está compilado e pronto para uso, e só depende de onde será usado.

## Execução na *BitDogLab*

- **1°:** coloque o seu ***Raspberry*** em modo de ***bootsel***, clicando no botão branco na placa e reiniciando a mesma.

- **2°:** copie o arquivo `.uf2` que foi gerado na pasta `build` para o seu ***Raspberry*** (ele aparecerá como um armazenamento externo, um Pen-Drive com nome de RPI-RP2).

    - Após isso, rode o código e abra o monitor serial do vs code.

- **3°:** Está pronto, dê os comandos no monitor serial ou clique nos botões.



## Comandos e funcionalidades disponívels:
**Comando via teclado no monitor serial**: letras maiúsculas ou minúsculas, ou números de 0 a 9 (caractere único por comando), após digitados são mostrados no display e na matriz de leds no caso de números;

Botão `A`: acende ou apaga o led RGB em verde, juntamente com retorno em texto no display do sucesso da ação;

Botão `B`: acende ou apaga o led RGB em azul, juntamente com retorno em texto no display do sucesso da ação;

## Vídeo Ensaio mostrando execução na placa ***BitDogLab***:

Clique em ***[link do video](https://youtu.be/t77KQAaVPeo)*** para visualizar o vídeo ensaio do projeto.
