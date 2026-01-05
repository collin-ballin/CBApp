bitfile_path = "C:\\Users\\alephnul\\Documents\\LabVIEW Data\\SPADMK2\\FPGA Bitfiles\\spadccumk2_FPGATarget_DSPTesting_coCfkuMbrXo.lvbitx"
resource_path = "rio://172.22.11.2/RIO0"

from nifpga import Session
import math
import time
import matplotlib.pyplot as plt
import numpy as np

def measure_raw(session):
    counts = [int(x) for x in session.registers['Counts'].read()];
    cycles = int(session.registers['CYCLES'].read());
    
    return counts, cycles

def start_measure(session, window = 1):
    print("Starting measurement")
    enable_ctrl = session.registers['ENABLE']
    clear_ctrl = session.registers['CLEAR']
    window_ctrl = session.registers['Conicidence Window']

    # Start by clearing the registers, verify they are cleared
    enable_ctrl.write(data=False)
    clear_ctrl.write(data=True)
    window_ctrl.write(window)
    
    counts, cycles = measure_raw(session)
    if (cycles != 0) or any([x != 0 for x in counts]):
        raise Exception('Clear/Stop failed.')
    
    
        
    # Remove clear condition, then enable counts
    clear_ctrl.write(data=False)
    enable_ctrl.write(data=True)
    
def finish_measure(session):
    enable_ctrl = session.registers['ENABLE']
    
    # Pause counts
    enable_ctrl.write(data=False)
    time.sleep(0.1)

    
    print("Measurement complete")
    
    # Pausing occurs and completes immediately, so it is already safe to measure the values:
    return measure_raw(session)

def resetAndMeasure(session, duration):
    session.reset()
    session.run()
    
    # Start the measurement
    start_measure(session, 50000)
    
    # Wait duration specified
    time.sleep(duration)
   
    # End the measurement
    return finish_measure(session)

with Session(bitfile=bitfile_path, resource=resource_path) as session:
    print(resetAndMeasure(session, 1))
    
    
