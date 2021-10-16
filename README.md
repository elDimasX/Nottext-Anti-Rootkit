# Nottext-Anti-Rootkit

# Anti-Rootkit
Essa ferramenta serve para verificar arquivos / processos / serviços ocultos, que não poderiam ser acessados através do user-mode, as funções funcionam porque são executadas em kernel-mode<br/>

![2021-10-16](https://user-images.githubusercontent.com/51800283/137601125-c831d027-6bc9-46a1-88d3-a8958e0082cf.png)
![2021-10-16 (1)](https://user-images.githubusercontent.com/51800283/137601114-80b0be85-caa4-403b-8f76-5da960029d79.png)
![2021-10-16 (2)](https://user-images.githubusercontent.com/51800283/137601115-928905f6-5e68-473f-9563-81ada937e729.png)
![2021-10-16 (3)](https://user-images.githubusercontent.com/51800283/137601118-da867b95-da0d-4494-ada5-115379cf6121.png)
![2021-10-16 (4)](https://user-images.githubusercontent.com/51800283/137601122-1c6d9651-3265-46fc-a02a-4eb2eaa304ad.png)
![2021-10-16 (7)](https://user-images.githubusercontent.com/51800283/137601188-96f3ac96-2899-4f77-96dc-b5ab09859249.png)


# Modo de teste
Para testar o Nottext Anti-Rootkit, você precisa ativar o Windows em modo de teste (testsigning on), e depois, executar o "Nottext Anti-Rootkit.exe", ele fará todo o resto

# Aberto a sugestões
Se você tiver alguma ideia de como posso melhorar meu anti-rootkit, por favor me avise

# Atualizações recentes

# 1.0.0.5
Essa versão inclu novas funcionalidades e melhorias
- Remoção de pastas melhorado
- Bloquear novos processos de serem executados
- IRP para listar pasta (permitindo localizar arquivos ocultos por rootkit)
- Melhoria na renomeação de arquivos
- Opção de mostrar erros
- Uma pequena melhoria na interface

# 1.0.0.4
Essa versão inclui novas funcionalidades e melhorias
- Remoção de arquivos melhorado
- Melhoria na finalização de processos (podendo finalizar processos com gancho SSDT)
- Verifica os serviços instalados na máquina
- Verifica os drivers/imagens carregados no sistema
- Iniciar/Parar serviço de driver apartir do kernel
- Interface simplificada

## 1.0.0.3
Essa versão inclui correção de bugs e melhorias:
- Remoção de arquivos melhorado
- Remoção de arquivos durante o boot
- Correção de bugs
- Interface melhorada

## 1.0.0.1
Essa versão inclui atualização de correção de bugs:
- Ocultação de processos
- Melhoria na remoção de arquivos
- Desligar ou reiniciar o computador a partir do kernel
- Renomear de arquivo
- Copiar arquivo

## 1.0.0.0
- Deletação de arquivos
- Listar arquivos e pastas
- Listar processos
- Terminar processos e bloquea-los
