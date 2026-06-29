import numpy as np
from scipy import stats
import pyqtgraph as pg
from PyQt5 import QtWidgets, QtCore
import collections

class StatsVisualizer(QtWidgets.QWidget):
    def __init__(self, max_samples=5000):
        super().__init__()
        self.max_samples = max_samples
        self.data = collections.deque(maxlen=self.max_samples)
        self.bins = 256
        
        self.layout = QtWidgets.QVBoxLayout(self)
        self.win = pg.GraphicsLayoutWidget(show=True, title="Quantum Entropy Real-Time Analysis")
        self.win.resize(1000, 800)
        self.layout.addWidget(self.win)
            
        # graph 1 - histogram (Distribution)
        self.p1 = self.win.addPlot(title="Quantum Entropy Distribution")
        self.hist_curve = self.p1.plot(stepMode="center", fillLevel=0, fillOutline=True, brush=(42, 183, 202, 150))
        self.p1.setLabel('bottom', 'Byte Value', units='0-255')
        self.p1.setLabel('left', 'Frequency')
        
        self.win.nextRow()
        
        # graph 2 - autocorrelation (Temporal Independence)
        self.p2 = self.win.addPlot(title="Autocorrelation (Lag Analysis)")
        self.p2.setLabel('bottom', 'Lag', units='tau')
        self.p2.setLabel('left', 'Correlation', units='R')
        self.p2.setYRange(-0.2, 1.1)
        self.corr_curve = pg.PlotCurveItem(pen=pg.mkPen('y', width=2))
        self.p2.addItem(self.corr_curve)
        
        # Confidence Intervals
        self.upper_line = pg.InfiniteLine(pos=0, angle=0, pen=pg.mkPen('r', style=QtCore.Qt.DashLine))
        self.lower_line = pg.InfiniteLine(pos=0, angle=0, pen=pg.mkPen('r', style=QtCore.Qt.DashLine))
        self.p2.addItem(self.upper_line)
        self.p2.addItem(self.lower_line)
        # Timer for periodic updates
        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.update_plot)
        self.timer.start(33) # ~30 FPS
    
    @QtCore.pyqtSlot(list)
    def handle_new_data(self, new_bytes):
        self.data.extend(new_bytes)
        print(f"Buffer size: {len(self.data)} / {self.max_samples}")

    def calculate_metrics(self):
        if len(self.data) < 50:
            return None, None, None, None
            
        # Histogram
        y, x = np.histogram(self.data, bins=self.bins, range=(0, 256))
        
        # Chi-Squared
        expected = len(self.data) / self.bins
        chi_stat, p_value = stats.chisquare(y)
        
        # Autocorrelation (Lag 0 to 40)
        y_corr = np.array(self.data) - np.mean(self.data)
        norm = np.sum(y_corr**2)
        if norm == 0: return x, y, None, None
        
        lags = np.arange(40)
        corrs = [np.sum(y_corr[lag:] * y_corr[:-lag]) / norm if lag > 0 else 1.0 for lag in lags]
        
        return x, y, p_value, (lags, corrs)

    def update_plot(self):
        if not self.data:
            return
            
        x, y, p_value, corr_data = self.calculate_metrics()
        
        # Update histogram
        if x is not None:
            self.hist_curve.setData(x, y)
            title = f"Distribution (N={len(self.data)})"
            if p_value is not None:
                title += f" | Chi-sq P-value: {p_value:.4f}"
            self.p1.setTitle(title)
            
        # Update autocorrelation
        if corr_data is not None:
            lags, corrs = corr_data
            self.corr_curve.setData(lags, corrs)
            
            # Update confidence lines
            conf = 1.96 / np.sqrt(len(self.data))
            self.upper_line.setPos(conf)
            self.lower_line.setPos(-conf)
            
