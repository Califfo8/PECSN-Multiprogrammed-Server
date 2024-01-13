import pandas as pd
import numpy as np
import statsmodels.api as sm
import matplotlib.pyplot as plt
import scipy.stats as st
import matplotlib.patches as mpatches
from matplotlib.lines import Line2D

class DataAnalysis:
    def __init__(self, factor_names, data_name, value_name, num_module):
        percorso = "Data_DA/" + data_name
        # Privati
        self._raw_data = pd.DataFrame(pd.read_excel(percorso))
        self.num_module = num_module
        self.modules = {
            "System.server.cpu": 0,
            "System.server.hd": 1,
            "System.webServer": 2
        }
        self.modules_names = ['CPU', 'HDD', 'WS']
        # Converto l'indice delle repliche in un vettore di interi
        indice_repliche = self._raw_data['Replication'].values
        for i in range(len(indice_repliche)):
            indice_repliche[i] = int(indice_repliche[i].replace("#", ""))
        # Trovo il numero di ripetizioni
        self._num_repliche = max(indice_repliche) + 1
        # Pubblici
        self.factor_names = factor_names
        self.num_factors = len(factor_names)
        self.num_experiments = self._raw_data.shape[0]//self._num_repliche
        self.scenario_matrix = None
        self.value_name = value_name

    def _strfact_to_vet(self, vet, car='$'):
        vet = vet.replace(car, "")
        factors_vet = vet.rsplit(", ")
        factors = np.zeros(shape=self.num_factors)
        if self.num_factors == 0:
            print("ERRORE: Non è stato inizializzato il numero di fattori")
            return factors

        for i in range(self.num_factors):
            pos, val = factors_vet[i].rsplit("=")
            pos = int(pos)
            val = float(val)
            factors[pos] = val
        return factors

    def find_factor_values(self):

        # Creo la matrice dei fattori
        values = np.zeros(shape=(self.num_factors, 3))

        # Trovo i fattori
        for i in range(self.num_factors):
            valori = self.scenario_matrix[self.factor_names[i]]
            values[i][0] = min(valori)
            values[i][2] = max(valori)
            for v in valori:
                if v > values[i][0] and v < values[i][2]:
                    values[i][1] = v
                    break

        # Creo la tabella
        self.factors = pd.DataFrame(values, columns=["Low", 'Medium', "High"], index=self.factor_names)
        # Esporto in excel
        self.factors.to_excel("Result_DA/Factors.xlsx")

    def compute_CI(self, r):
        values = r['Value'].values
        ci = st.norm.interval(alpha=0.99,
                         loc=np.mean(values),
                         scale=st.sem(values))
        delta = ci - np.mean(values)
        return delta[1]

    def create_scenario_matrix(self):
        ordered_data = self._raw_data.sort_values(['Measurement'])
        group_data = ordered_data.groupby(['Measurement', 'Module'])
        mean_table = group_data['Value'].mean()
        ci = group_data.apply(self.compute_CI)
        df = pd.concat([mean_table, ci], axis=1, join='inner')
        df = df.reset_index()
        confi = []
        for i in range(self.num_experiments):
            confi.append(self._strfact_to_vet(df['Measurement'][i]))

        df1 = pd.DataFrame(confi, columns=self.factor_names)
        df.drop(['Measurement'], axis=1)
        df = pd.concat([df1, df], axis=1, join='inner')
        df.rename(columns={"Value": self.value_name}, inplace=True)
        df.rename(columns={"0": "Confidence Interval"}, inplace=True)
        self.scenario_matrix = df
        self.find_factor_values()
        df.to_excel("Result_DA/scenario_analysis.xlsx")


    def create_scenario_matrix_worse(self):
        ordered_data = self._raw_data.sort_values(['Measurement'])
        trh = ordered_data['Value'].values
        mean_value = np.zeros(shape=(self.num_experiments, 1))
        confidence_interval = np.zeros(shape=(self.num_experiments, 2))
        confi = []
        module = []
        for i in range(self.num_experiments):
            trh_scenario = trh[i * self._num_repliche: (i+1) * self._num_repliche]

            confi.append(self._strfact_to_vet(ordered_data['Measurement'][i * self._num_repliche]))
            module.append(ordered_data['Module'][i * self._num_repliche])
            print(ordered_data['Module'][i * self._num_repliche], ordered_data['Value'][i * self._num_repliche])
            mean_value[i] = np.mean(trh_scenario)
            ci = st.norm.interval(alpha=0.99,
                             loc=mean_value[i],
                             scale=st.sem(trh_scenario))
            confidence_interval[i][0] = -(ci[1][0]-mean_value[i])
            confidence_interval[i][1] = ci[1][0] - mean_value[i]

        # Racchiudo tutto in un dataframe
        df1 = pd.DataFrame(confi, columns=self.factor_names)
        df2 = pd.DataFrame(mean_value, columns=[self.value_name])
        df3 = pd.DataFrame(confidence_interval, columns=['neg_Delta_CI', 'Delta_CI'])
        df = pd.concat([df1, df2, df3], axis=1, join='inner')
        self.scenario_matrix = df
        self.find_factor_values()
        df.to_excel("Result_DA/scenario_analysis.xlsx")

    def plot_two_factor_graph(self, A, B, fixed_value='Medium', auto_color=True):
        A_name = self.factor_names[A]
        B_name = self.factor_names[B]

        # Verifico i parametri fixed
        fixed = []
        for i in range(self.num_factors):
            if i != A and i != B:
                fixed.append([i, self.factors[fixed_value][i]])
        # Prendo le righe aventi i valori dei parametri voluti
        cond_factor_one = self.scenario_matrix[self.factor_names[fixed[0][0]]] == fixed[0][1]
        cond_factor_two = self.scenario_matrix[self.factor_names[fixed[1][0]]] == fixed[1][1]
        righe_selezionate = self.scenario_matrix[cond_factor_one & cond_factor_two]
        #Ordino in base al secondo parametro
        righe_selezionate.sort_values(by=[B_name, 'Module'], inplace=True, ignore_index=True)
        # Creo il grafico con le rette
        # plt.clf()
        colori = ['tab:blue', 'tab:orange', 'tab:green']
        ind_c = -1
        cont = -1
        line_style = ['solid', 'dashed', 'dotted']
        patches = []
        for i in range((righe_selezionate.shape[0]//3)):
            x_y = righe_selezionate[[A_name, self.value_name, 'Module']][i*3:(i+1)*3]
            valori_B = righe_selezionate[B_name]
            modulo = x_y['Module'][i*3]
            x_y.sort_values(by=[self.factor_names[A]], inplace=True)
            # Necessari per la colorazione personalizzata
            cont = cont+1
            ind_c = cont // 3
            if cont == 9:
                cont = -1
            # Verifico se voglio la legenda e i colori personalizzati o no
            if auto_color:
                label = B_name + " = " + str(valori_B[i * 3])
                plt.plot(x_y[A_name], x_y[self.value_name], label=label, linestyle=line_style[self.modules[modulo]])
            else:
                plt.plot(x_y[A_name], x_y[self.value_name], linestyle=line_style[self.modules[modulo]], color=colori[ind_c])
                if cont%3==0:
                    label = B_name + " = " + str(valori_B[i * 3])
                    patches.append(mpatches.Patch(color=colori[ind_c], label=label))

        titolo = A_name + " vs " + B_name
        nome_file = titolo.replace(" ", "_")
        nome_file = nome_file + ".png"
        titolo = titolo + ": " + fixed_value
        plt.title(titolo)
        plt.grid()
        # Inserisco nella legenda i valori delle linee
        if auto_color:
            plt.legend()
        else:
            for i in range(len(line_style)):
                patches.append(Line2D([0], [0], color='black', ls=line_style[i], label=self.modules_names[i]))
            plt.legend(bbox_to_anchor=(1.05, 1), loc=2, borderaxespad=0., handles=patches)
        plt.xlabel(A_name)
        plt.ylabel(self.value_name)

        return nome_file

    def test_Normal_HP(self):
        plt.clf()
        # Trasformo la matrice dei residui in un vettore
        values = self._raw_data['Value'].values
        values = values[0:50]
        values = np.sort(values)
        # Crea il QQ plot
        fig = sm.qqplot(values, line='r')
        plt.grid()
        plt.savefig('Result_DA/Normal_HP_test.png')

        # Istogramma dei residui
        plt.clf()
        width_bucket = 5
        min_n = int(min(values))
        max_n = int(max(values))
        n_bins = np.arange(min_n, max_n + width_bucket, width_bucket)
        plt.hist(values, bins=n_bins)
        plt.clf()
    #max_scale_y limita l'asse y da 0 a max_scale. Se max_scale è -1 la scala la sceglie python
    def full_plot(self, max_scale_y, auto_color = True):
        fixed_value = ["Low", 'Medium', "High"]
        subplot_index = 1
        for i in range(self.num_factors):
            for j in range(i+1, self.num_factors):
                for k in fixed_value:
                    plt.subplot(1, 3, subplot_index)
                    if max_scale_y != -1:
                        plt.ylim(0, max_scale_y)
                    plt.gcf().set_size_inches(20, 6)
                    nome_file = self.plot_two_factor_graph(i, j, k, auto_color)
                    subplot_index += 1
                plt.tight_layout()

                plt.savefig('Result_DA/Graphs/' + nome_file, dpi=300)
                plt.clf()
                subplot_index = 1

