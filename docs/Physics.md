
# Physical Foundations of Quantum Randomness

## 1. The Photon Beam Splitter Model

In a classical world, a light beam hitting a 50/50 beam splitter (BS) simply divides its intensity. However, at the quantum level, when a **single photon** is incident on the BS, it does not divide. Instead, it enters a state of **quantum superposition**.

### Mathematical Representation

Let the input state of the photon be . The beam splitter acts as a unitary operator $\hat{U}_{BS}$ on the Hilbert space of the photon's spatial modes. For a 50/50 splitter, the transformation is:

$$\hat{U}_{BS} = \frac{1}{\sqrt{2}} \begin{pmatrix} 1 & i \\ i & 1 \end{pmatrix}$$

When a photon enters from mode $|0\rangle$, the resulting state is:

$$|\psi\rangle = \hat{U}_{BS} |0\rangle = \frac{1}{\sqrt{2}} (|R\rangle + i|T\rangle)$$

Where:

* $|R\rangle$ is the reflected state.
* $|T\rangle$ is the transmitted state.
* The factor $i$ represents a $\frac{\pi}{2}$ phase shift upon reflection (required for unitarity).

## 2. Collapse and Inherent Randomness

According to the **Born Rule**, the probability of detecting the photon in either mode is given by:

$$P(R) = | \langle R | \psi_{out} \rangle |^2 = \left| \frac{1}{\sqrt{2}} \right|^2 = 0.5$$
$$P(T) = | \langle T | \psi_{out} \rangle |^2 = \left| \frac{i}{\sqrt{2}} \right|^2 = 0.5$$

This randomness is **not** due to a lack of information (like a coin toss), but is a fundamental property of nature. The act of measurement by the photodetector causes the "collapse of the wave function", yielding a definitive binary outcome: 0 or 1.

## 3. Real-world Imperfections and Entropy Processing

In our QRNG system, the hardware might introduce **bias** (e.g.,  due to detector dark counts or thermal noise). This is where the **Von Neumann Extractor** implemented in our `vProcessingTask` becomes crucial.

By taking pairs of bits:

* $01 \rightarrow 0$ 
* $10 \rightarrow 1$
* $00, 11 \rightarrow \text{Discard}$

We ensure that the final bitstream is unbiased, even if the underlying physical source has a slight drift, preserving the cryptographic integrity of the output.

## 4. Alternative Model: Shot Noise in Zener Diodes

In systems where optical setups are impractical, we utilize the **Quantum Tunneling** effect in a Zener diode operating in the breakdown region.

### The Physics of Shot Noise

Shot noise arises due to the discrete nature of electric charge. When electrons cross a potential barrier (like the p-n junction in a diode), the arrival times of individual electrons follow a **Poisson Process**.

The spectral density of the current fluctuations  is given by the Schottky formula:
$$S_i = 2qI$$

Where:

* $q\ (1.6 \times 10^{-19}$ C) is the elementary charge ( C).
* $I$ is the average DC current through the diode.

### From Current to Bits

The voltage fluctuations $V(t)$ across a load resistor $R$ are sampled by the ADC. Since each electron's passage is a statistically independent event, the resulting noise is white and follows a Gaussian distribution in the macroscopic limit (due to the Central Limit Theorem), but its origin remains fundamentally quantum-mechanical (tunneling).

In our system, the ADC captures these micro-volt fluctuations, and the **Von Neumann algorithm** ensures that any bias in the sampling hardware is removed, leaving only the pure stochastic noise of the tunneling electrons.

---

# Mathematical Formalism of Zener-Based Quantum Randomness

While the Single Photon model is a discrete two-state system, the **Zener Diode QRNG** relies on the collective stochastic behavior of electron populations governed by quantum tunneling and Poisson statistics.

## 1. Quantum Tunneling Probability ($P_T$)

In a Zener diode, electrons transition from the valence band to the conduction band by tunneling through the depletion region's potential barrier $V(x)$. Using the **WKB (Wentzel–Kramers–Brillouin) approximation**, the transmission probability  for a single electron is:

$$P_T \approx \exp \left( -2 \int_{x_1}^{x_2} \sqrt{\frac{2m^*}{\hbar^2} (V(x) - E)} \, dx \right)$$

For a semiconductor with a bandgap $E_g$ under a high electric field $E_{field}$, this simplifies to:

$$P_T \propto \exp \left( -\frac{\pi \sqrt{m^*} E_g^{3/2}}{2\sqrt{2} q \hbar E_{field}} \right)$$

Where $m^*$ is the effective mass of the electron. This process is fundamentally probabilistic; there is no classical "cause" for a specific electron to tunnel at a specific time.

## 2. Statistical Nature: The Poisson Process

Because the tunneling probability $P_T$ is extremely low and there is a vast reservoir of available electrons, the number of electrons  crossing the junction in a given time interval  follows a **Poisson distribution**:

$$P(n = k) = \frac{\lambda^k e^{-\lambda}}{k!}$$

The parameter $\lambda$ represents the average number of electrons per interval, related to the DC current by $\lambda = \frac{I \cdot \Delta t}{q}$. The inherent variance in this process ($\sigma^2 = \lambda$) is the source of our entropy.

## 3. Spectral Density: Schottky’s Formula

The instantaneous current $I(t)$ is a summation of discrete charge pulses:

$$I(t) = \sum_{i} q \delta(t - t_i)$$

Where $t_i$ are the random arrival times. In the frequency domain, the **Power Spectral Density (PSD)** of these fluctuations, known as **Shot Noise**, is given by Schottky's formula:

$$S_I(f) = 2q\bar{I}$$

Where $\bar{I}$ is the average current. This white noise spectrum is flat across frequencies, indicating that the noise power is uniformly distributed.

This result is purely quantum-mechanical. In a classical continuum limit where the elementary charge $q \to 0$, the noise power vanishes. The presence of this noise is a direct macroscopic manifestation of the quantized nature of charge and the stochastic nature of tunneling.

## 4. Comparison: Single Photon vs. Zener Tunneling

| Feature | Single Photon (Beam Splitter) | Zener Diode (Shot Noise) |
| --- | --- | --- |
| **Hilbert Space** | Two-level system | Continuum of charge states |
| **Mechanism** | Wavefunction collapse (Born Rule) | Statistical tunneling of discrete charges |
| **Observables** | Binary detection (0 or 1) | Continuous voltage fluctuations $V(t)$ |
| **ADC Role** | Digital pulse counting | Sampling the variance of $V(t)$ |
