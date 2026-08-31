Battery Level Estimation
========================

The battery level is estimated by feeding the voltage and current through a Kalman Filter based on the Thevenin 1RC Model.

Battery Model
-------------

The battery model is the Thevenin 1RC Model : 

.. math::

    V(t) = V_{oc}(t) - I(t)R_{int} - V_{pol}(t)

.. math::

    \frac{dV_{pol}}{dt} = \frac{I(t)}{C_{pol}} - \frac{V_{pol}}{R_{pol}C_{pol}}

- :math:`V(t)` : The measured voltage of the battery
- :math:`I(t)` : The measured current out of the battery
- :math:`V_{oc}(t)` : Open Circuit Voltage, the voltage that the battery would have if disconnected, this is what we are trying to estimate
- :math:`R_{int}` : The internal resistance of the battery
- :math:`V_{pol}(t)` : The polarisation voltage
- :math:`R_{pol}` : The polarisation resistance
- :math:`C_{pol}` : The polarisation capacitance

Kalman Filter
-------------

The kalman filter is based on the `wikipedia description <https://en.wikipedia.org/wiki/Kalman_filter>`__ and is configured as follows :

**State (x)** : :math:`\begin{bmatrix} V_{oc}(t) \\ V_{pol}(t) \end{bmatrix}`



**Command (u)** : :math:`\begin{bmatrix} I(t) \end{bmatrix}`


**Measure (z)** : :math:`\begin{bmatrix} V(t) + I(t)R_{int}  \end{bmatrix}`

Kalman Matrices
_______________

.. math:: 
    
    F = \begin{bmatrix} 1 & 0 \\ 0 & 1 - \frac{dt}{R_{pol}C_{pol}} \end{bmatrix}

.. math:: 
    
    B = \begin{bmatrix} -dt*\frac{dV_{OC}}{dQ} \\ \frac{dt}{C_{pol}} \end{bmatrix}

.. math:: 
    
    Q = \begin{bmatrix} dt^2 \sigma_{I}^2 \frac{dV_{OC}}{dQ}^2 + \sigma_{V_{oc}}^2 & - \frac{dt^2}{C_{pol}} \sigma_{I}^2 \frac{dV_{OC}}{dQ} \\ 
    - \frac{dt^2}{C_{pol}} \sigma_{I}^2 \frac{dV_{OC}}{dQ} & \frac{dt^2\sigma_{I}^2}{C_{pol}^2} + \sigma_{V_{pol}}^2 \end{bmatrix}

.. math:: 
    
    H = \begin{bmatrix} 1 & -1\end{bmatrix}

.. math:: 
    
    R = \begin{bmatrix} \sigma_{V}^2 + R_{int}^2 \sigma_{I}^2 \end{bmatrix}

.. note::

    :math:`\frac{dV_{OC}}{dQ}` corresponds to the small voltage drop caused by a small capacity usage of the battery when the open circuit voltage is :math:`V_{OC}`, this is calculated using Voltage-Charge lookup tables


Kalman Settings
_______________

- :math:`R_{int}` : The internal resistance of the battery
- :math:`R_{pol}` : The polarisation resistance
- :math:`C_{pol}` : The polarisation capacitance

----

- :math:`\sigma_{V}` : The voltage measure standard deviation
- :math:`\sigma_{I}` : The current measure standard deviation
- :math:`\sigma_{V_{oc}}` : The standard deviation of the :math:`V_{oc}` process noise
- :math:`\sigma_{V_{pol}}` : The standard deviation of the :math:`V_{pol}` process noise
- :math:`\sigma_{0V_{oc}}` : The initial standard deviation of the :math:`V_{oc}` estimation
- :math:`\sigma_{0V_{pol}}` : The initial standard deviation of the :math:`V_{pol}` estimation

----

- :math:`CV_{LUT}` : The charge / voltage lookup table of the battery (used to compute :math:`\frac{dV_{OC}}{dQ}`)
- :math:`C_{max}` : The capacity of the battery (used to compute :math:`\frac{dV_{OC}}{dQ}`)