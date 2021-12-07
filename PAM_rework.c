#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>
#include "PAM_rework.h"

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
void affecterPointsMedoideProche(P_Point student, P_Cluster clusters, int *distance[], int numObjs, int K)
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
Cluster selectBestClusterMedoids(P_Point student, int *distance[], Cluster cluster, int numObjs)
{
  int i, j = 0;
  float totalDist = 0;
  float tmpTotalDist = 0;
  int *medoidPoints = (P_Point)malloc(sizeof(Student) * cluster.taille);
  int medoid = cluster.medoid;
  for (i = 0; i < numObjs; i++)
  {
    if (student[i].cluster->medoid == cluster.medoid)
    {
      medoidPoints[j] = i;
      j++;
    }
  }
  for (i = 0; i < numObjs; i++)
    totalDist += distance[max(i, medoid)][min(i, medoid)];

  for (i = 0; i < cluster.taille; i++)
  {
    tmpTotalDist = 0;
    int tmpMedoid = medoidPoints[i];

    for (j = 0; j < numObjs; j++)
      tmpTotalDist += distance[max(j, tmpMedoid)][min(j, tmpMedoid)];

    if (tmpTotalDist < totalDist)
    {
      cluster.medoid = tmpMedoid;
      totalDist = tmpTotalDist;
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

  printf("READ FILE\n");
  fflush(stdout);

  FILE *fp;
  Student student1[50];
  int i = 0;
  char line[100];

  // ************	START PAM	************

  int clusterMedoid[K];
  int numObjs = (sizeof(student1) / sizeof(int) / 10);
  printf("\nNombre de point %d \n", numObjs);

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
      affecterPointsMedoideProche(student, clusters, distance, numObjs, K);
      printf("\n");

    

    /*  2(b)   Pour tout objet représentatif m (et donc le cluster associé) et pour tout objet o dans D
               qui ne soit pas un objet représentatif.                                   */

    
    /*  2(b)i. Soit E le coût actuel du partitionnement, calculer le coût S de la partition 
               dans laquelle o est un objet représentatif à la place de m                */

      for (i = 0; i < K; i++)
      {
        tmpClusters[i] = selectBestClusterMedoids(student, distance, clusters[i], numObjs);
      }

      for (i = 0; i < K; i++)
        if (tmpClusters[i].medoid != clusters[i].medoid)
        {
          changeTest = 1;
          break;
        }
      if (changeTest)
      {
        clusters = tmpClusters;
        for (i = 0; i < K; i++)
        {
          printf("\n cluster[%d] -> %d membres\n", i, clusters[i].taille);
        }
      }

    } while (changeTest == 1);

    for (i = 0; i < K; i++)
    {
      printf("  nouveau cluster[%d]: %d\n", i, clusters[i].medoid);
    }
    printf("\n");

    for (i = 0; i < K; i++)
    {
      int medoid = clusters[i].medoid;
      for (j = 0; j < numObjs; j++)
      {
        if (student[j].cluster->medoid == medoid)
        {
          printf("%s\n",student[j].name);
        }
      }
      printf("\n");
      for (i = 0; i < numObjs; i++)
      {
        //printf("  [%s]: %d\n", student[i].name, student[i].cluster->medoid);
      }
      printf("\n");
    }

    // ************	END PAM		************

    return 0;
  }
}
