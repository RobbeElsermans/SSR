Energy awareness is an important topic of our system. Energy Awareness includes detecting the available energy present, determining the tasks that can be executed with the available energy, and optimizing payload transfers and tasks scheduling to provide an optimal usage of power.

Therefore, their is a need to develop algorithms that provide these functionalities. In this section, multiple ideas will be presented an explained. The goal is to implement as many as possible.

## Optimal Task Schedule
The selected tasks exhibit varying consumption profiles. Specifically, the current consumption for each task can be calculated based on its duration and the average current consumption during that duration. By determining the remaining energy available in the energy storage, it becomes possible to allocate tasks or combinations of tasks according to the energy constraints.

For instance, consider three tasks where each subsequent task consumes 50% more energy than the previous one. If Task 1 consumes 2 units of energy, Task 2 would consume 3 units (50% more than Task 1), and Task 3 would consume 4.5 units (50% more than Task 2). By evaluating the total energy available, you can decide which tasks or combinations of tasks can be performed without exceeding the energy limit.

> A basic Task scheduler is implemented where the voltage of the super capacitor determines the sequence of tasks.
## Optimal Transmission
When two rovers are within a certain distance where their measured data is highly likely to be similar, it is inefficient to redundantly transmit this data over a long range to the base station. To optimize energy usage, a mechanism can be employed allowing rovers to detect each other's presence within a specific range. In this range, the rovers should be able to estimate their relative proximity, not necessarily as a precise metric value but as a qualitative factor (e.g., "close" or "far").

Once both rovers detect proximity, they need to exchange information to decide **who** will transmit and **what** data will be transmitted.

1. **Who will transmit:**  
    The decision can be based on the remaining energy of each rover. The rover with the highest available energy should be selected as the long-range communicator to preserve the energy of the lower-energy rover.
    
2. **What will be transmitted:**  
    The data sent to the base station can either be:
    
    - The value measured by the lower-energy rover.
    - The value measured by the higher-energy rover.
    - An averaged value of both rovers' sensed data, depending on the situation.

**Example with 🥔️ numbers:**

- Rover A has **80% energy remaining**, and Rover B has **30% energy remaining**.
- Rover A and Rover B are in proximity, and their measured data values are:
    - Rover A: **25 🥔️**
    - Rover B: **24 🥔️**

**Decision process:**

- **Who transmits:** Rover A (because it has higher energy remaining).
- **What to transmit:**
    - Option 1: Transmit **24 🥔️** (data from the low-energy Rover B).
    - Option 2: Transmit **25 🥔️** (data from the high-energy Rover A).
    - Option 3: Transmit the **average value**: $(25+24)/2=24.5🥔️$.

This approach reduces transmissions and preserves the energy of the rover with lower reserves, ensuring more efficient data communication over long distances.

> This energy aware algorithm is not jet implemented in the code. Although, preparations are made within the BLE-scan and beacon code in terms of the exchanged data.

## Optimal Waiting
When executing certain tasks, a significant portion of time is used by waiting for modules to process their commands. This idle waiting time results in wasted, valuable energy. To optimize energy efficiency, waiting periods should be minimized, and during unavoidable waits, the system should consume as little energy as possible.

In code, certain techniques can help achieve this, such as using function references. These function reference can later be used to create custom delay blocks where we can put the MCU in a low power mode when waiting. Or even doing some custom tasks if a certain delay is requested.

It can be implemented in C as follows
```c
typedef void (*delay_callback_t)(uint32_t time);
void bleDelayCallback(delay_callback_t dc_fp);
extern delay_callback_t _delay_callback;
```
1. **`typedef void (*delay_callback_t)(uint32_t time);`**
    
    - This defines a new type named `delay_callback_t`, which represents a function pointer. The function pointer type corresponds to functions that return `void` and take a single `uint32_t` argument (representing time).
2. **`void bleDelayCallback(delay_callback_t dc_fp);`**
    
    - This declares a function named `bleDelayCallback` that takes a parameter `dc_fp` of type `delay_callback_t`. This means `bleDelayCallback` accepts a function pointer to a function that matches the `delay_callback_t` signature.
3. **`extern delay_callback_t _delay_callback;`**
    
    - This declares an external variable `_delay_callback` of type `delay_callback_t`. It acts as a global function pointer, which can be used within the program to reference a callback function for delay operations. The `extern` keyword indicates that the actual definition of `_delay_callback` exists in another file.

To conclude:
- `delay_callback_t` defines a standardized function pointer type for delay callbacks.
- `bleDelayCallback` sets up a mechanism to pass a delay callback function to the program.
- `_delay_callback` is a global pointer used to reference the active delay callback function within the code base.

This gives the opportunity to define our own abstract delay statement which we can optimize in terms of energy consumption and even bring it to another level and not wait, instead do some other tasks

> Each library that is written uses these delay function references to optimize energy consumption when a delay is needed.


## Optimal Timeout
In two-way communication, it is often necessary to wait for the other party to process a command or respond. The same principles as discussed in **Optimal Waiting** apply here: minimize energy consumption during the waiting period by operating in the lowest power mode possible.

However, waiting for a response indefinitely can be risky, as the other party might fail to respond due to errors, disconnection, or other issues. To address this, implementing a **timeout mechanism** ensures that the waiting period is limited, preventing excessive delays and allowing the system to take corrective action if no response is received.

A small example:

- **Scenario:**  
    A rover sends a command to a sensor to collect data. The sensor typically takes **2 seconds** to process and respond, but due to potential failures, it may not respond at all.
    
- **Implementation:**
    
    - The rover initiates a wait state in **low-power mode** after sending the command.
    - A **timeout** of **5 seconds** is set.
    - If a response is received within **2 seconds**, the rover processes it and exits the wait state.
    - If no response is received after **5 seconds**, the timeout triggers an error-handling routine (e.g., retransmit the command, log the issue, or move on).

This approach minimizes wasted energy during the wait and ensures timely recovery from communication failures.

> Optimal timeouts are implemented in each possible hanging loop waiting period.