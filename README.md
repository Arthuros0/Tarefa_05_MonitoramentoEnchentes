# 🌊 Estação de Alerta de Enchente

Projeto desenvolvido para a **Tarefa 05** da residência de Sistemas Embarcados com FreeRTOS, utilizando a **BitDogLab com RP2040**.

---

## 🎯 Objetivo

Simular uma estação de monitoramento de enchentes que utiliza o **joystick analógico** para representar:

- **Nível do rio** (eixo Y)
- **Volume de chuva** (eixo X)

O sistema identifica automaticamente situações de risco e emite **alertas visuais e sonoros**, tornando-se acessível e relevante para aplicações reais.

---

## ⚙️ Funcionamento

### Modos de operação:

#### ✅ Modo Normal
- LED RGB verde
- Buzzer desativado
- Matriz de LEDs exibe círculo
- Display OLED mostra dados ambientais

#### ⚠️ Modo Alerta
Ativado automaticamente se:
- **Nível do rio ≥ 70%**
- **Volume de chuva ≥ 80%**

Neste modo:
- LED RGB fica vermelho
- Buzzer emite sinal sonoro intermitente
- Matriz de LEDs mostra um ícone de perigo
- Display destaca o estado de alerta com os valores

---

## 🧵 Estrutura do sistema (FreeRTOS)

| Tarefa | Função |
|--------|--------|
| `vJoystickTask` | Obtêm os valores do joystick |
| `vProcessingSensorTask` | Interpreta os dados do joystick alterando o valor do sensores |
| `vDisplayTask` | Atualiza o display OLED com dados e status |
| `vLedTask` | Controla a cor do LED RGB |
| `vBuzzerTask` | Emite sons de alerta |
| `vMatrixLedTask` | Mostra ícones conforme o estado do sistema |

---

## 🔁 Comunicação entre tarefas

A comunicação entre tarefas foi feita **exclusivamente por filas (`xQueue`)**, como exigido pela tarefa. Cada tarefa recebe comandos ou dados pela sua fila correspondente.

---

## 🧩 Periféricos utilizados

- **Joystick analógico**
- **Display OLED (I2C)**
- **Matriz de LEDs 5x5**
- **Buzzer ativo**
- **LED RGB**

---


