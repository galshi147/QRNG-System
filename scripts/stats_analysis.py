import numpy as np
from scipy import stats
import matplotlib.pyplot as plt

class StatsVisualizer:
    def __init__(self, max_samples=2000):
        self.data = []
        self.max_samples = max_samples
        self.bins = 256
        self.fig, (self.ax_hist, self.ax_corr) = plt.subplots(2, 1, figsize=(10, 8))
        plt.tight_layout(pad=4.0)
        plt.ion()  # Enable interactive mode
        
    def add_data(self, new_bytes):
        self.data.extend(new_bytes)
        # keep constant time window (Moving Window)
        if len(self.data) > self.max_samples:
            self.data = self.data[-self.max_samples:]
    
    def calculate_autocorrelation(self, max_lag=40):
        if len(self.data) < max_lag + 10:
            return None, None
        y = np.array(self.data) - np.mean(self.data)
        norm = np.sum(y**2)
        # calculate autocorrelation for each Lag
        correlations = [np.sum(y[lag:] * y[:-lag]) / norm if lag > 0 else 1.0 
                        for lag in range(max_lag)]
        return range(max_lag), correlations
    
    def perform_chi_squared(self):
        if len(self.data) < 500: # need enough samples for significance
            return None, None
        observed, _ = np.histogram(self.data, bins=self.bins, range=(0, 255))
        expected = np.full(self.bins, len(self.data) / self.bins) 
        # Calculate Chi-squared
        chi_stat, p_value = stats.chisquare(observed, expected)
        return chi_stat, p_value

    def update_plot(self):
        if not self.data:
            return
            
        self.ax_hist.clear()
        self.ax_hist.hist(self.data, bins=self.bins, range=(0, 255), color='#2ab7ca', edgecolor='black', alpha=0.7)
        chi_stat, p_value = self.perform_chi_squared()
        title = f"Quantum Entropy Distribution (N={len(self.data)})"
        if p_value is not None:
            title += f"\nChi-sq: {chi_stat:.2f}, P-value: {p_value:.4f}"
            self.ax_hist.set_title(title)
            self.ax_hist.set_xlabel("Byte Value (0-255)")
            self.ax_hist.set_ylabel("Frequency")
            self.ax_hist.grid(axis='y', alpha=0.3)
        
        lags, corr = self.calculate_autocorrelation()
        if lags is not None and corr is not None:
            self.ax_corr.clear()
            self.ax_corr.stem(lags, corr, basefmt=" ")
            self.ax_corr.set_ylim(-0.2, 1.1)
            self.ax_corr.set_title("Autocorrelation (Lag Analysis)")
            self.ax_corr.set_xlabel("Lag (τ)")
            self.ax_corr.set_ylabel("Correlation R(τ)")
            # add limit lines for significance (95% Confidence Interval)
            conf = 1.96 / np.sqrt(len(self.data))
            self.ax_corr.axhline(conf, color='r', linestyle='--', alpha=0.5)
            self.ax_corr.axhline(-conf, color='r', linestyle='--', alpha=0.5)

        
        self.fig.canvas.draw()
        self.fig.canvas.flush_events()