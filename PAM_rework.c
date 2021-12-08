#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>

// Structure d'un cluster
typedef struct Cluster
{
  int medoid;
  int taille;
} Cluster;
typedef Cluster *P_Cluster;

// Structure d'un point
typedef struct Students
{
  char name[12];
  int sindex;
  int u1;
  int u2;
  int u3;
  int u4;
  P_Cluster cluster;
} Student;
typedef Student *P_Point;

// retourne le min de 2 valeur
int min(int i, int j)
{
  int m = (i < j) ? i : j;
  return m;
}
// retourne le max de 2 valeur
int max(int i, int j)
{
  int m = (i > j) ? i : j;
  return m;
}

// fonction qui determine la distance entre 2 objets
int Manhattan_distance(Student P1, Student P2)
{
  int u = abs(P1.u1 - P2.u1) + abs(P1.u2 - P2.u2) + abs(P1.u3 - P2.u3) + abs(P1.u4 - P2.u4);
  return u;
}

// fonction d'initialisation des K-Medoids
void initialiseClusters(P_Point student, P_Cluster clusters, int numObjs, int K)
{
  int i;
  for (i = 0; i < K; i++)
  {
    clusters[i].medoid = i;
    clusters[i].taille = 0;
  }
}

// fonction initialisation des distances de touts les student
void initialiseDistance(int *distance[], P_Point student, int numObjs)
{
  int i, j;
  for (i = 0; i < numObjs; i++)
  {
    for (j = 0; j < i + 1; j++)
    {
      distance[i][j] = Manhattan_distance(student[i], student[j]);
    }
  }
}

// fonction affectation des objets au Medoid le plus proche
void affectClosestKmedoid(P_Point student, P_Cluster clusters, int *distance[], int numObjs, int K)
{
  int i, j;
  for (i = 0; i < K; i++)
    clusters[i].taille = 0;
  for (i = 0; i < numObjs; i++)
  {
    Cluster *cluster = &clusters[0];
    int mid = cluster->medoid;
    for (j = 1; j < K; j++)
    {
      Cluster *tmpCluster = &clusters[j];
      int tmpmid = tmpCluster->medoid;
      if (distance[max(i, tmpmid)][min(i, tmpmid)] < distance[max(i, mid)][min(i, mid)])
      {
        cluster = tmpCluster;
        mid = cluster->medoid;
      }
    }
    student[i].cluster = cluster;
    cluster->taille++;
  }
}

// fonction retournant le meilleur groupe de clusters
Cluster getBestClusterMed(P_Point student, int *distance[], Cluster cluster, int numObjs)
{
  int i, j = 0;
  int E = 0;
  int S = 0;
  int *medoidPoints = (P_Point)malloc(sizeof(Student) * cluster.taille);
  int medoid = cluster.medoid;
  int best_cost;
  for (i = 0; i < numObjs; i++)
  {
    if (student[i].cluster->medoid == cluster.medoid)
    {
      medoidPoints[j] = i;
      // printf("%d\n",medoidPoints[j]);
      j++;
    }
  }
  for (i = 0; i < numObjs; i++)
    E += distance[max(i, medoid)][min(i, medoid)];

  for (i = 0; i < cluster.taille; i++)
  {
    S = 0;
    int tmpMedoid = medoidPoints[i];

    for (j = 0; j < numObjs; j++)
      S += distance[max(j, tmpMedoid)][min(j, tmpMedoid)];

    if (S < E)
    {
      cluster.medoid = tmpMedoid;
      E = S;
    }
    if (best_cost > E)
    {
      best_cost = min(S, E);
      E = best_cost;
      printf(" Cout Total: %d\n", best_cost);
    }
  }

  free(medoidPoints);
  return cluster;
}

int main(int argc, char **argv)
{

  /*                                                          */
  int K = 4;                                     // Clusters K
  FILE *csvFile = fopen("harrypotter.txt", "r"); // Input File
  /*                                                          */

  printf("READING FILE\n");
  fflush(stdout);

  FILE *fp;
  Student student1[50];
  int i = 0;
  char line[100];

  // ************	START PAM	************

  int numObjs = (sizeof(student1) / sizeof(int) / 10);
  printf("\nNombre de points %d \n", numObjs);

  P_Point student = (P_Point)malloc(numObjs * sizeof(Student));
  P_Cluster clusters = (P_Cluster)malloc(K * sizeof(Cluster));

  if (csvFile)
  {
    int index = 0;
    while (fgets(line, sizeof(line), csvFile) != NULL && i < 50)
    {
      // Extract Nom
      char *pStr = strtok(line, ";");
      if (pStr != NULL)
      {
        strcpy(student[index].name, pStr);
      }
      // Extract Loyauté
      pStr = strtok(NULL, ";");
      if (pStr != NULL)
      {
        student[index].u1 = atoi(pStr);
      }
      // Extract Courage
      pStr = strtok(NULL, ";");
      if (pStr != NULL)
      {
        student[index].u2 = atoi(pStr);
      }
      // Extract Sagesse
      pStr = strtok(NULL, ";");
      if (pStr != NULL)
      {
        student[index].u3 = atoi(pStr);
      }
      // Extract Malice
      pStr = strtok(NULL, ";");
      if (pStr != NULL)
      {
        student[index].u4 = atoi(pStr);
        student[index].sindex = index;
      }
      index++;
    }

    /* 1. Choisir arbitrairement (aléatoirement) K objets dans D comme représentation initiale (ou graine) des clusters.*/

    bool is;
    unsigned int randseed;

    for (i = 0; i < K; i++)
    {
      do
      {
        is = true;
        randseed = rand() % (numObjs); // randseed correspond à la graine des clusters

        /*  check if cluster has not already been picked  */
        for (int j = 0; j < i; j++)
        {
          if (clusters[j].medoid == randseed)
          {
            is = false;
            break;
          }
        }
      } while (is == false);
      clusters[i].medoid = randseed;
    }
    printf("\n");

    // Affiche tout les K-Medoids.

    printf("K-Medoids : \n\n");

    for (i = 0; i < K; i++)
    {
      printf("   cluster[%d]: %d\n", i, clusters[i].medoid);
    }
    printf("\n");
    /*  2(a) Affecter chaque objet non représentatif dans le cluster associé à l’objet représentatif qui est
             le plus similaire (le plus proche au sens de la distance ou la similarité retenue).                */

    int j, changeTest;
    int *distance[numObjs];
    for (i = 0; i < numObjs; i++)
    {
      distance[i] = (int *)malloc((i + 1) * sizeof(int));
    }
    initialiseClusters(student, clusters, numObjs, K);
    initialiseDistance(distance, student, numObjs);

    do
    {
      changeTest = 0;
      P_Cluster tmpClusters = (P_Cluster)malloc(sizeof(Cluster) * K);

      affectClosestKmedoid(student, clusters, distance, numObjs, K);
      printf("\nClustering...\n\n");

      for (i = 0; i < K; i++)
      {
        tmpClusters[i] = getBestClusterMed(student, distance, clusters[i], numObjs);
      }
      printf("\n");

      for (i = 0; i < K; i++)
        if (tmpClusters[i].medoid != clusters[i].medoid)
        {
          changeTest = 1;
          break;
        }
      if (changeTest)
      {
        clusters[i] = tmpClusters[i];
        for (i = 0; i < K; i++)
        {

          printf("\n cluster[%d] -> %d membres\n", i, clusters[i].taille);
        }
      }

    } while (changeTest == 1);
    printf("K-Medoids :\n\n");
    for (i = 0; i < K; i++)
    {

      printf("  nouveau cluster[%d]: %d\n", i, clusters[i].medoid);
    }
    printf("\n");

    // ************	END PAM		************

    return 0;
  }
}