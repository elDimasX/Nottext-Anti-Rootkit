# Nottext-Anti-Rootkit

# Anti-Rootkit
Essa ferramenta serve para verificar arquivos / processos / serviços ocultos, que não poderiam ser acessados através do user-mode, as funções funcionam porque são executadas em kernel-mode, tendo todo o acesso possível<br/>

![2023-03-01 (1)](https://user-images.githubusercontent.com/51800283/222134062-e3989e61-c25e-413f-b8d1-cdced741dd1e.png)
![2023-03-01 (5)](https://user-images.githubusercontent.com/51800283/222171111-330baa5c-4460-49c6-8648-faf26db54daa.png)
![2021-12-27 (10)](https://user-images.githubusercontent.com/51800283/147514175-8de1cb0f-7656-4cc9-8cce-ed433527106f.png)
![2021-12-27 (11)](https://user-images.githubusercontent.com/51800283/147514177-970cbe74-db89-408d-959d-22ae3146e52e.png)
![2021-12-27 (12)](https://user-images.githubusercontent.com/51800283/147514181-0b8cadf1-05ce-4142-bd79-ab1fa23bb8e5.png)
![2023-03-01 (6)](https://user-images.githubusercontent.com/51800283/222171355-db445746-1fbe-42f9-9251-578c086b428c.png)

# Modo de teste
Para testar o Nottext Anti-Rootkit, você precisa ativar o Windows em modo de teste (testsigning on), execute ele, e se não estiver em modo de teste, ele exibirá um aviso para ativar, após ativar, reinicie o computador

# Aberto a sugestões
Se você tiver alguma ideia de como posso melhorar meu anti-rootkit, por favor me avise

# Atualizações recentes

## 1.0.0.6 (2)
Essa versão só corrige alguns bugs e remove algumas funcionalidades
- Corrigido bug que causava travamentos aleatóriamente quando iniciava o driver
- Verifica a assinatura de drivers para verificar se eles são legítimos ou não
- Corrigido a tela de serviço, deixando o local do arquivo e argumentos separados
- Verifica se os serviços são um rooktit com arquivo escondido

## 1.0.0.6
Essa versão inclui novas funcionaliades e melhorias
- Auto-defesa (impede que processos finalizem o Nottext Anti-Rootkit)
- Opção de suspender ou resumir qualquer processo
- Destaca drivers ocultos (que provavelmente são rootkits)
- Destaca processos ocultos (que provavelmente são rootkits)
- Botão de desativar o modo de teste do Windows
- Muda o nome do programa automaticamente (muda o nome do Windows Forms para tentar impedir que rootkits o detectem)
- Removido a proteção de processos (não finalizei essa parte ainda)

## 1.0.0.5
Essa versão inclui novas funcionalidades e melhorias
- Remoção de pastas melhorado
- Bloquear novos processos de serem executados
- IRP para listar pasta (permitindo localizar arquivos ocultos por rootkit)
- Melhoria na renomeação de arquivos
- Opção de mostrar erros
- Uma pequena melhoria na interface

## 1.0.0.4
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
