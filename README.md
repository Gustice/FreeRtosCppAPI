# Humble FreeRtosCppAPI Abstraction

Simplify feature-rich FreeRTOS utilities by wrapping them in C++ classes.

## Purpose

This library aims to provide an easy to use FreeRTOS abstraction layer for C++ projects.
Especially it encapsulates all bulky freeRTOS functions in nicer syntax and removes
the necessity of casting void arguments to the correct type again.

## Examples

### Tasks

Of course you are able to create a task in quite few steps but passing all the required arguments
and casting optional task parameter to its intended type again, is annoying and tends to be error prone.
However, are you aware that a task must not return and if it still return it should be deleted?


<table>
<tr> <th>Vanilla FreeRTOS</th> <th>Humble Abstraction</th></tr> 
<tr>
<td style="vertical-align: top"> 

```c++
int main(void) {
    T1Param p;
    xTaskCreate(vTask1, "Task 1", 
        1000, &p, 1, NULL);
}

void vTask1(void *pvParam) {
    // parameter needs to be casted
    T1Param *param = (T1Param *)pvParam;
    while (true) {
        bool error = doSomeWork();
        if (error) {
            break;
        }
        vTaskDelay(ticksToDelay);
    }
    vTaskDelete(NULL);
}
```
</td>
<td style="vertical-align: top"> 

```c++
using namespace fos;
void setupTasks() {
    static TaskT<Queue<TxMsg> &> 
        queueTask(paramTask, txQueue, 
        "Tick");
}

void paramTask(Queue<TxMsg> &queue) {
    while (true) {
        auto msg = queue.dequeue(); 
        doSomeWork(msg);
    }
}
```
</td>
</tr>
</table>

In this Abstraction type casting and conditional deleting is done in the class body.
Your tasks expose only the relevant parts.
The best part is where you already get the correct argument signature, and cannot pass a wrongly typed argument.

### Semaphore

The advantages in case of semaphore are not that significant compared to vanilla FreeRTOS.

<table>
<tr> <th>Vanilla FreeRTOS</th> <th>Humble Abstraction</th></tr> <tr>
<td style="vertical-align: top"> 

```c++
int main(void) {
    binSem = xSemaphoreCreateBinary();
    //...
}

static void vProducerTask(void) {
    while(true) {
        // Work
        xSemaphoreGive(binSem);
    }
}

static void vHandlerTask(void *pvParameters) {
    while(true) {
        xSemaphoreTake(binSem, portMAX_DELAY);
        // Work
    }
}
```
</td>
<td style="vertical-align: top"> 

```c++
using namespace fos;
Semaphore blinkSem;

static void Producer() {
    while (true) {
        // Work
        blinkSem.give();
    }
}

static void Consumer() {
    while (true) {
        blinkSem.take();
        // Work
    }
}
```
</td>
</tr>
</table>

The only benefit is that the whole syntax is cleaner and easier to develop because as you have the instance on hand intellisense can support you finding the correct method on it very quick.

## Used Development

### Espressif

- Tested on: `seeed_xiao_esp32c3` board
- pIO-platform: `Espressif 32` `V 6.4.0`
- framework: `espidf`

### STM32

- Tested on: `genericSTM32F401CC` board
- pIO-platform: `ST STM32` `17.2.0`
- framework: `espidf` 
- additional library: `STM32duino FreeRTOS`


## ToDo

It would be nice to provide more of the api calls like the static construction of tasks and message queues and events during interrupts