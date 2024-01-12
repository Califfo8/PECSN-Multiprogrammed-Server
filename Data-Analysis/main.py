import pandas as pd

from DataAnalysis import DataAnalysis
if __name__ == '__main__':
    PATH_FISSO = "D:/Documenti/OneDrive/Universita/LM-Computer Engineering/PECSN/PECSN-Project/Factorial-Analysis"
    pd.options.display.max_rows = 9999

    #Salvare i dati da Omnet++ in csv for spreadsheet
    factor_names1 = ["CPUr", "HDr", "WSr", "p1", "p2"]
    factor_names2 = ["A", "B"]
    #FA = FactorialAnalysis(factor_names1, "Sim3-Repl50.xlsx")
    #FA.compute_result_matrix()
    #FA.test_hp_and_confidence(0.95)

    # Data Analysis
    #Nota: nei grafici per avere i colori delle linee diverse e la legenda è necessario commentare due riga nella
    # funzione plot_two_factor_graph. Soltanto quando si calcola il throughput
    DA = DataAnalysis(["μHD", "μWS", "p1", "p2"], "utilization_scenario.xlsx", 'Utilization', 3) #throughput
    DA.create_scenario_matrix()
    DA.full_plot(1) #Max y value throughput = 240  Max y value utilization = 1
    DA.test_Normal_HP()
