import pandas as pd
import numpy as np
import statsmodels.api as sm
import matplotlib.pyplot as plt
import scipy.stats as st
import math as m

class FactorialAnalysis:

    def __init__(self, factor_names, data_name):
        percorso = "Data/" + data_name
        self.raw_data = pd.DataFrame(pd.read_excel(percorso))
        # Privati
        self._sign_matrix = None
        self._result_matrix = None
        self._subset_matrix = None
        self._num_factors = 0
        self._num_repeat = 0
        self._mean_error_matrix = None
        self._column_names = ["I"]
        self._column_names.extend(factor_names)
        self._somme_coefficenti = None
        self.SSE = 0
        # Pubblici
        self.factor_names = factor_names
        self.complete_sign_matrix = None
        self.complete_result_matrix = None
        self.complete_matrix = None
        self.factors = None

    def _strfact_to_vet(self, vet, car='$'):
        vet = vet.replace(car, "")
        factors_vet = vet.rsplit(", ")
        factors = np.zeros(shape=self._num_factors)
        if self._num_factors == 0:
            print("ERRORE: Non è stato inizializzato il numero di fattori")
            return factors

        for i in range(self._num_factors):
            pos, val = factors_vet[i].rsplit("=")
            pos = int(pos)
            val = float(val)
            factors[pos] = val
        return factors

    def _powerset(self, lst):
        if not lst:
            return [[]]
        exclude_first = self._powerset(lst[1:])
        include_first = [[lst[0]] + x for x in exclude_first]
        return exclude_first + include_first

    def find_low_high_values(self):
        valori = self.raw_data['Measurement']
        factors_vet = valori[0].rsplit(", ")
        self._num_factors = len(factors_vet)
        # Creo la matrice dei fattori

        values = np.zeros(shape=(self._num_factors, 2))
        for i in range(self._num_factors):
            values[i] = [1000000000000000000000000000000000000000000000000000, -1]

        # Inserisco i valori
        for x in valori:
            factors_vet = self._strfact_to_vet(x)
            for i in range(self._num_factors):
                values[i][0] = min(values[i][0], factors_vet[i])
                values[i][1] = max(values[i][1], factors_vet[i])

        # Creo la tabella
        self.factors = pd.DataFrame(values, columns=["Low", "High"], index=self.factor_names)

        # Esporto inexcel
        self.factors.to_excel("Result/Factors.xlsx")

    def _create_basic_matrix(self):
        # Inizializzo le variabili
        self.find_low_high_values()
        valori = self.raw_data['Measurement']
        results = self.raw_data['Value'].values
        # Converto l'indice delle repliche in un vettore di interi
        indice_repliche = self.raw_data['Replication'].values
        for i in range(len(indice_repliche)):
            indice_repliche[i] = int(indice_repliche[i].replace("#", ""))

        # Trovo il numero di ripetizioni
        self._num_repeat = max(indice_repliche) + 1

        # Creo la matrce per i segni avente come righe il numero di esperimenti effettivi (no ripetizioni)
        sign_matrix = np.zeros(shape=(int(valori.shape[0] / self._num_repeat), self._num_factors))
        # Creo la matrice per i risultati delle simulazioni
        result_matrix = np.zeros(shape=(sign_matrix.shape[0], self._num_repeat))
        # Converto la stringa dei valori nelle combinazioni di segni
        # per ogni riga
        j = -1
        for i in range(valori.shape[0]):
            # Se è la prima simulazione aggiungo la dovuta riga
            if indice_repliche[i] == 0:
                j = j + 1
                # Prendo i valori attribuiti ai fattori
                factor_vet = self._strfact_to_vet(valori[i])
                # Sostituisco i valori con i segni
                indici_bassi = (factor_vet == self.factors['Low'])
                indici_alti = (factor_vet == self.factors['High'])
                sign_matrix[j][indici_bassi] = -1
                sign_matrix[j][indici_alti] = 1

                # Inserisco il corrispettivo risultato nella matrice omonima
                result_matrix[j][0] = results[i]
            elif indice_repliche[i] > 0:
                # Se invece sono repliche aggiungo una colonna di y
                result_matrix[j][indice_repliche[i]] = results[i]

        self._sign_matrix = sign_matrix
        self._result_matrix = result_matrix

    def _create_subset_matrix(self):
        self._create_basic_matrix()
        indici = range(0, len(self.factor_names))
        subsets = self._powerset(indici)
        subsets.sort()
        subsets = subsets[1:len(subsets)]  # Tolgo il subset vuoto

        subset_matrix = np.zeros(shape=(self._sign_matrix.shape[0], len(subsets) - self._num_factors))
        ind = -1
        # per ogni subset
        for i in range(len(subsets)):
            # A parte i valori singoli
            if len(subsets[i]) == 1:
                continue
            ind = ind + 1
            # Calcolo il segno di ogni riga
            for r in range(self._sign_matrix.shape[0]):
                new_sign = 1
                name = ""
                for factor in subsets[i]:
                    new_sign = self._sign_matrix[r][factor] * new_sign
                    name = name + self.factor_names[factor]
                subset_matrix[r][ind] = new_sign
            self._column_names.append(name)
        self._subset_matrix = subset_matrix

    def _create_mean_error_matrix(self):
        # Per ogni riga calcolo la media dei risultati
        medie_rep = np.zeros(shape=(self._result_matrix.shape[0], 1))
        for i in range(self._result_matrix.shape[0]):
            medie_rep[i] = np.mean(self._result_matrix[i])

        # Per ogni riga calcolo gli errori dalla media
        error_y_ym = np.zeros(shape=(self._result_matrix.shape[0], self._result_matrix.shape[1]))
        for i in range(self._result_matrix.shape[1]):
            error_y_ym[:, i] = self._result_matrix[:, i] - medie_rep[:, 0]

        # Concateno il tutto
        self._mean_error_matrix = np.concatenate((medie_rep, error_y_ym), axis=1)

    def compute_complete_matrix(self):
        self._create_subset_matrix()
        self._create_mean_error_matrix()
        I = np.ones(shape=(self._sign_matrix.shape[0], 1))

        # Compatto tutto nella matrice finale
        self.complete_sign_matrix = np.concatenate((I, self._sign_matrix, self._subset_matrix), axis=1)
        self.complete_result_matrix = np.concatenate((self._result_matrix, self._mean_error_matrix), axis=1)
        # Aggiungo i nomi per le colonne dei risultati
        for i in range(self._num_repeat):
            strin = "y(" + str(i + 1) + ")"
            self._column_names.append(strin)
        # Aggiungo il nome per la media
        self._column_names.append("s.m. y")
        # Aggiungo i nomi per gli errori
        for i in range(self._mean_error_matrix.shape[1]-1):
            strin = "y(" + str(i + 1) + ")-ym"
            self._column_names.append(strin)
        # Esporto il file
        df1 = pd.DataFrame(self.complete_sign_matrix, columns=self._column_names[0:self.complete_sign_matrix.shape[1]], dtype=int)
        df2 = pd.DataFrame(self.complete_result_matrix, columns=self._column_names[self.complete_sign_matrix.shape[1]:], dtype=float)
        df = pd.concat([df1, df2], ignore_index=False, axis=1)
        self.complete_matrix = df
        df.to_excel("Result/complete_matrix.xlsx")

    def compute_result_matrix(self, truncate=False):
        self.compute_complete_matrix()

        # Per ogni colonna calcolo il totale e i rispettivi coefficenti
        somme_coefficenti = np.zeros(shape=(2, self.complete_sign_matrix.shape[1]))
        for i in range(self.complete_sign_matrix.shape[1]):
            somme_coefficenti[0][i] = np.sum(self.complete_sign_matrix[:, i] * self._mean_error_matrix[:, 0])
        # Divido ogni somma per 2^k
        somme_coefficenti[1] = [x / pow(2, self._num_factors) for x in somme_coefficenti[0]]

        # Faccio lo stesso con gli errori
        error_sum_coef = np.zeros(shape=(2, self._result_matrix.shape[1]))
        for i in range(1, self._mean_error_matrix.shape[1]):
            error_sum_coef[0][i-1] = np.sum(self._mean_error_matrix[:, i])
        # Divido ogni somma per 2^k
        error_sum_coef[1] = [x / pow(2, self._num_factors) for x in error_sum_coef[0]]

        # Calcolo i SSX
        SSX = pow(somme_coefficenti[1], 2) * pow(2, self._num_factors) * self._num_repeat
        # Tolgo l'SSI, derivato da I
        SSX[0] = 0
        #Calcolo SSE
        SSE = 0
        for j in range(1, self._mean_error_matrix.shape[1]):
            for i in range(self._mean_error_matrix.shape[0]):
                SSE = SSE + pow(self._mean_error_matrix[i, j], 2)
        self.SSE = SSE
        SSX = np.concatenate((SSX, [SSE]))
        # Determino SST
        SST = np.sum(SSX)
        # Calcolo la variazione
        variation = (SSX / SST) * 100

        # Creo il Dataframe per i risultati
        self._somme_coefficenti = somme_coefficenti
        coefficent = np.concatenate((somme_coefficenti, error_sum_coef), axis=1)
        if truncate:
            coefficent = coefficent.astype(int)
        column_number = self.complete_sign_matrix.shape[1]
        names = self._column_names[0: column_number]
        names.extend(self._column_names[column_number + self._num_repeat + 1:])
        df_coefficent = pd.DataFrame(coefficent, index=["Sum", "mean(qi)"], columns=names)

        # Concateno il tutto per l'output
        if truncate:
            variation = variation.astype(int)
            SSX = SSX.astype(int)
        names = self._column_names[0:somme_coefficenti.shape[1]]
        names.append(self._column_names[len(self._column_names)-1])
        t = [SSX, variation]
        df = pd.DataFrame(t, index=["r*2^(k)*2qi^2", "variation"], columns=names)
        result = pd.concat([df_coefficent, df], ignore_index=False, axis=0)

        #Esporto tutto in excel
        result.to_excel("Result/variation.xlsx")

        #Creo un recap riassuntivo per evidenziare meglio i risultati
        ordered_result = result.sort_values(result.last_valid_index(), axis=1, ascending=False)
        ordered_result.to_excel("Result/variation_recap.xlsx")

    def test_Normal_HP(self):
        plt.clf()
        # Trasformo la matrice dei residui in un vettore
        residual = self._mean_error_matrix[:, 1:].flatten()
        residual = np.sort(residual)
        # Crea il QQ plot con la bisattrice line added to plot
        fig = sm.qqplot(residual, line='r')
        plt.grid()
        plt.savefig('Result/Normal_HP_test.png')

    def test_Homoscedasticity_HP(self):
        plt.clf()
        # Creo il vettore dell'asse x ripetendo r volte i corrispettivi valori medi dei residui
        x= []
        for mean in self._mean_error_matrix[:,0]:
            for n in range(self._num_repeat):
                x.append(mean)
        #Il vettore degli ascisse sarà pari ai residui
        residual = self._mean_error_matrix[:, 1:].flatten()
        plt.scatter(x, residual)
        plt.grid()
        plt.savefig('Result/Homoscedasticity_test.png')

    def compute_confidence_interval(self, confidenza):
        n = pow(2, self._num_factors) * (self._num_repeat - 1)
        alpha_mezzi = (1-confidenza)/2
        error_variance = self.SSE/n
        percentile = abs(st.t.ppf(alpha_mezzi, n))
        delta = percentile * m.sqrt((error_variance/(pow(2, self._num_factors) * self._num_repeat)))
        # Calcolo l'intervallo per ogni qi
        all_qi = self._somme_coefficenti[1]
        confidence_interval_matrix = []
        for qi in all_qi:
            confidence_interval_matrix.append([qi, qi - delta, qi + delta])

        row_names = self._column_names[0:self._somme_coefficenti.shape[1]]
        confidence_interval = pd.DataFrame(confidence_interval_matrix, columns=["qi", "min", "max"], index=row_names)
        confidence_interval.to_excel("Result/confidence_interval.xlsx")

    def test_hp_and_confidence(self, confidenza):
        self.test_Normal_HP()
        self.test_Homoscedasticity_HP()
        self.compute_confidence_interval(confidenza)

    def help(self):
        Text = "Questo programma utilizza come input un file di excel avente lo stesso formato delle tabelle di Omnet++, il file deve \n"
        Text = Text + "essere inserito nella cartella Data. Inoltre si necessita di un vettore stringa contenente il nome dei fattori in studio, \n"
        Text = Text +"l'ordine dei nomi deve essere identico all'associazione che compie Omnet++ con i numeri (ex. $0) della colonna Measurement.\n"
        Text = Text +"Tutti i risultati sono espressi attraverso una serie di file excel nella cartella Result."
        print(Text)