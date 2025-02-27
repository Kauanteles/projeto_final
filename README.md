Resumo do Projeto
O projeto implementa um sistema de autenticação com senha utilizando a placa Raspberry Pi Pico. O sistema inclui a manipulação de uma matriz de LEDs, um display SSD1306, LEDs de feedback individual e botões que utilizam interrupções, pull-up e debounce para garantir a leitura correta dos cliques. O usuário pode criar uma senha e, posteriormente, autenticar-se digitando a senha correta, com feedback visual na matriz de LEDs e no display OLED.

Instruções de Uso
Para usar o software, siga os seguintes passos:

1° Passo: Clonar o Repositório
Clone o repositório para o seu computador.

2° Passo: Configurar o Ambiente
Abra o projeto com o VSCode.

A extensão do CMake irá criar a pasta build automaticamente com os arquivos de compilação.

Caso a pasta build não seja gerada, crie uma pasta com o nome build e execute o seguinte comando dentro dela:

bash
Copy
cmake ..
Esse comando irá gerar os arquivos de compilação necessários.

3° Passo: Compilar o Firmware
Execute a compilação do firmware usando a extensão do Raspberry Pi Pico no VSCode.

Agora, o firmware está compilado e pronto para uso.

Execução no Raspberry Pi Pico
1° Passo: Entrar em Modo de Boot
Coloque o Raspberry Pi Pico em modo de bootsel:

Segure o botão BOOTSEL na placa.

Conecte o Raspberry Pi Pico ao computador via USB.

Solte o botão BOOTSEL.

2° Passo: Carregar o Firmware
Copie o arquivo .uf2 gerado na pasta build para o Raspberry Pi Pico (ele aparecerá como um armazenamento externo chamado RPI-RP2).

3° Passo: Executar o Projeto
Abra o monitor serial no VSCode.

O sistema estará pronto para uso. Siga as instruções exibidas no display OLED e utilize os botões para interagir com o sistema.

Funcionalidades Disponíveis
Botão A: Criar Senha
Pressione o botão A para criar uma nova senha.

Digite a senha desejada no terminal serial (máximo de 6 caracteres).

A senha será armazenada e o sistema exibirá uma animação "V" na matriz de LEDs para confirmar a criação.

Botão B: Inserir Senha
Pressione o botão B para inserir uma senha.

Digite a senha no terminal serial.

Se a senha estiver correta, o sistema exibirá uma animação "V" na matriz de LEDs.

Se a senha estiver incorreta, o sistema exibirá uma animação "X" na matriz de LEDs.

Feedback Visual
Matriz de LEDs:

"V" (verde): Senha correta.

"X" (vermelho): Senha incorreta.

Display OLED:

Exibe mensagens de instrução e feedback para o usuário.

Vídeo de Demonstração
Clique em link do vídeo para visualizar o vídeo de demonstração do projeto. (https://youtu.be/lt4p2NqllEU)
