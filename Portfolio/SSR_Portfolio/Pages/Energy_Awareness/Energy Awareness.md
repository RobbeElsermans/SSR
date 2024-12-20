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
    - Option 3: Transmit the **average value**: (25+24)/2=24.5(25 + 24) / 2 = 24.5(25+24)/2=24.5 🥔️.

This approach reduces redundant transmissions and preserves the energy of the rover with lower reserves, ensuring more efficient data communication over long distances.

## Optimal Waiting

## Optimal Timeout