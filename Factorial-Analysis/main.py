import pandas as pd
from FactorialAnalysis import FactorialAnalysis
if __name__ == '__main__':
    PATH_FISSO = "D:/Documenti/OneDrive/Universita/LM-Computer Engineering/PECSN/PECSN-Project/Factorial-Analysis"
    pd.options.display.max_rows = 9999

    #Salvare i dati da Omnet++ in csv for spreadsheet
    factor_names1 = ["Nc", "CPUr", "HDr", "WSr", "p1", "p2"]
    factor_names2 = ["A", "B"]
    FA = FactorialAnalysis(factor_names1, "prova1.xlsx")

    FA.compute_result_matrix()
    FA.test_hp_and_confidence(0.95)

