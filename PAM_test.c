#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

int Donnees[][2] = {{1, 1}, {2, 3}, {3, 9}, {9, 4}, {9, 8}, {10, 6}, {2, 8}, {10, 4}, {10, 3}, {7, 4}, {10, 7}, {5, 4}, {4, 4}, {6, 6}, {4, 2}, {7, 2}, {1, 5}, {3, 8}, {7, 2}, {1, 5}, {3, 8}, {7, 2}, {7, 8}, {4, 9}};

int Manhattan_distance(int xa, int ya, int xb, int yb){
    int j = abs(xb - xa) + abs(yb - ya);
    return j;
}

int main() {

    srand(time(NULL));
    const int K = 4;

    int clusterMedoids[K];
    int numClusters = (sizeof(Donnees)/sizeof(int)/2);
    printf("\nNombre de points : %d \n", numClusters);


    /* 1. Choisir arbitrairement (aléatoirement) K objets dans D comme représentation initiale (ou graine) des clusters.*/

    bool is;
    unsigned int randseed;
    int i;
    for( i = 0; i < K; i++) {
        do {
            is = true;
            randseed = rand() % (numClusters); // randseed correspond à la graine des clusters

            /*  check if cluster has not been already chosen  */
            for (int j = 0; j<i; j++) {
                if (clusterMedoids[j] == randseed) 
                {is = false; break;}
            }
        } while (is == false);
        clusterMedoids[i] = randseed;
    }
    printf("\n");

    // show all K objects of clusterMedoids.
    
    printf("K-Medoids : \n\n");
    
    for ( int i = 0; i < K; i++) {
        printf("  clusterMedoids[%d]: %d\n", i, clusterMedoids[i]);
    }
    printf("\n");

    /*  ||2.a|| Affecter chaque objet non représentatif dans le cluster associé à l’objet représentatif qui est
                    le plus similaire (le plus proche au sens de la distance ou la similarité retenue).                */
    
    bool loop = true;

    while (loop){

        loop = false;
        int mdMin; // Distance Manhattan minimale pour un point
        int nearest_cluster; // Cluster optimal ayant la distance Manhattan au point i la plus petite.        
    
        // Définit le data cluster vide
        int Cluster [K][numClusters];
        for (int i=0; i<K; i++){
            for (int j=0; j<numClusters; j++){
                Cluster [i][j] = -1;
            }
        }
        for (int i=0; i<numClusters;i++){
            mdMin = -1;
            // Recherche quel cluster à une distance de Manhattan la plus petite pour le point i
            for (int j= 0; j<K; j++){
                printf("Distance entre le centre %d et le point %d = %d\n", j, i, Manhattan_distance(Donnees[clusterMedoids[j]][0], Donnees[clusterMedoids[j]][1], Donnees[i][0], Donnees[i][1]));
                if (mdMin == -1 || mdMin < Manhattan_distance(Donnees[clusterMedoids[j]][0], Donnees[clusterMedoids[j]][1], Donnees[i][0], Donnees[i][1])) {
                    mdMin = Manhattan_distance(Donnees[clusterMedoids[j]][0], Donnees[clusterMedoids[j]][1], Donnees[i][0], Donnees[i][1]);
                    nearest_cluster = j;
                }
            }
            //printf("Cluster le plus proche : le %d\n", nearest_cluster);
            // Ajoute le point i au data cluster
            for (int j=0; j<numClusters; j++){
                if (Cluster [nearest_cluster][j] == -1 ) {Cluster [nearest_cluster][j] = i; break;}
            }
            
            // Exclu de la recherche les sommets des clusters
            for (int j=0; j<K; j++){
                if (i+1 == clusterMedoids[j]) {i++; j = -1;}
            }
            //printf("\n");
        }

        // Affiche les clusters
        for (int i=0; i<K; i++){
            printf("Cluster %d de centre %d : {", i , clusterMedoids[i]);
            for (int j=0; j<numClusters; j++){
                if (Cluster [i][j] != -1) {printf("%d, ", Cluster [i][j]);}
                else {printf("Null, ");}
                if (j+1 == numClusters) {
                    if (Cluster [i][j] != -1) {printf("%d}\n", Cluster [i][j]);}
                    else {printf("Null}\n");}
                }
            }
        }
        printf("\n");

        for (int i = 10; i > 0; i--) { 
            printf("\n %d |", i-1);
            for (int j = 1; j < 10; j++) {
                int nombre = -1;
                for (int l = 0; 1 < numClusters; l++) {
                    if (Donnees[1][0] == i && Donnees[1][1] == j) nombre = 1;
                }
            int cluster_bis = -1;
            if (nombre != -1) {
                for (int k = 0; k < K; k++) {
                    if (nombre == clusterMedoids[k]) cluster_bis = 99;
                    for (int m = 0; m<numClusters; m++) {
                        if (nombre == Cluster[k][m]) cluster_bis = k;
                    }
                }
            }
            if (cluster_bis != -1 && cluster_bis != 99) {
                if (cluster_bis > 9) printf("%d ", cluster_bis);
                else printf ("%d ", cluster_bis);
            }
            else if (cluster_bis != -1 ) printf("C ");
            else printf("   ");
            }
        }
        printf("\n  _________________________\n    1   2   3   4   5   6   7   8   9");
        printf("\n\n\n\n");

        for (int i = 0; i < K; i++) {


            // 2.b i calc E

            int E = 0;
            int j = 0;
            int meilleur = -1;
            while (Cluster[i][j] != -1) {
                E += Manhattan_distance(Donnees[clusterMedoids[i]][0], Donnees[clusterMedoids[i]][1], Donnees[Cluster[i][j]][0], Donnees[Cluster[i][j]][1]);
                //printf("Distnace entre %d et %d : %d \n", clusterMedoids[i], Cluster[i][j], Manhattan_distance(Donnees[clusterMedoids[i]][0], Donnees[clusterMedoids[i]][1], Donnees[Cluster[i][j]][0],Donnees[Cluster[i][j]][1]));
                j++;
            }
            printf("La somme des distances du cluster %d est %d \n", i, E);

            // 2.b ii calc S

            for (j=0; j < numClusters; j++) {
                int S = 0;
                int l = 0;
                while ( Cluster[i][1] != -1) {
                    S += Manhattan_distance(Donnees[j][0], Donnees[j][1], Donnees[Cluster[i][1]][0], Donnees[Cluster[i][1]][1]);
                    //printf("La distance totale depuis le point %d dans le cluster %d est de %d\n", j, i, S);
                }
                S += Manhattan_distance(Donnees[j][0], Donnees[j][0], Donnees[clusterMedoids[i]][0], Donnees[clusterMedoids[i]][1]);
                //printf("La distance totale depuis le point %d dans le cluster %d est de %d\n", j, i, S);
                if (S < E) {
                    E = S;
                    meilleur = j;
                    //printf("On modifie, le nouveau E vaut %d \n", S);
                }
            }

            // 3. Changer

            if (meilleur != -1) {
                clusterMedoids[i] = meilleur;
                loop = true;
                printf (" Le nouveau centre du cluster %d est %d \n", i, clusterMedoids[i]);
            }
        
        }
    }
    
    for (int i = 10; i > 0; i--) {
        printf("\n %d |", i-1);
        for (int j = 1; j < 10; j++) {
            int nombre = -1;
            for (int l = 0; 1 < numClusters; l++) {
                if (Donnees[1][0] == i && Donnees[1][1] == j) nombre =1;
            }
            if (nombre != -1) {
                if (nombre > 9) printf("%d  ", nombre);
            }
            else printf("   ");
        }
    }
    printf("\n   ___________________________\n  1  2  3  4  5  6  7  8  9");

    return 0;
}