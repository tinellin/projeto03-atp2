#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <Windows.h>

/**********************************GLOBAIS*************************************/
#define MAX_SIZE_OF_FILE 100

/*********************************ESTRUTURAS***********************************/
typedef struct SReferenceFrame
{
  int width;
  int height;
  int initialHeight;
  int *rgb;
} ReferenceFrame;

/********************************PROTOTIPOS************************************/
FILE *openFile(char *path, char *frame, char *i, int first);
ReferenceFrame createReferenceFrame(FILE *refFile, char *path);
void compareFrames(char *path, char *frame, ReferenceFrame ref);
char *parser(int ct);

/**********************************MAIN****************************************/
int main(void)
{
  char path[MAX_SIZE_OF_FILE];
  char frame[MAX_SIZE_OF_FILE];

  FILE *refFile;
  ReferenceFrame ref;

  printf("\n*----------------------------------------------------------------------------------*\n");
  printf("*------------------------ BEM VINDO AO SISTEMA DE VIGILANCIA ----------------------*\n");
  printf("*----------------------------------------------------------------------------------*\n\n");

  printf("ANTES DE TUDO: \n\n");

  printf("Verifique se:\n");
  printf("*------------------------------------------------------------------------------*\n");
  printf("|   1.     | A pasta dos frames do video estao na mesma pasta do sistema       |\n");
  printf("|   2.     | O formato dos frames e (.rgb)                                     |\n");
  printf("|   3.     | O nome do frame esta no formato yyy.rgb, onde y sao os numeros    |\n");
  printf("*------------------------------------------------------------------------------*\n\n");

  do
  {
    printf("Digite qual pasta esta os frames: ");
    scanf("%s", path);

    refFile = openFile(path, frame, "001", 0);

    if (refFile != NULL)
      break;
  } while (refFile == NULL);

  ref = createReferenceFrame(refFile, path);

  fclose(refFile);

  compareFrames(path, frame, ref);
}

/*********************************FUNCOES**************************************/
FILE *openFile(char *path, char *frame, char *i, int first)
{
  FILE *file;

  strcpy(frame, path);
  strcat(frame, "/");
  strcat(frame, i);
  strcat(frame, ".rgb");

  file = fopen(frame, "rb");

  if (first == 0 && file == NULL)
    printf("Erro: O arquivo binario nao pode ser aberto. Verifique-o.\n");

  strcpy(frame, " ");

  return file;
}

ReferenceFrame createReferenceFrame(FILE *refFile, char *path)
{
  ReferenceFrame ref;
  int i, j, k, px;

  if (strcmp(path, "video1") == 0)
  {
    ref.width = 190;
    ref.height = 240;
    ref.initialHeight = 120;
  }
  else
  {
    ref.width = 320;
    ref.height = 240;
    ref.initialHeight = 0;
  }

  ref.rgb = calloc((ref.width * ref.height * 3), sizeof(int));

  for (i = 0; i < ref.width; i++)
    for (j = ref.initialHeight; j < ref.height; j++)
      for (k = 0; k < 3; k++)
      {
        px = fgetc(refFile);
        *(ref.rgb + i * ref.height * 3 + j * 3 + k) = px;
      }

  printf("Arquivo salvo com sucesso. Verifique a pasta de destino.\n");

  return ref;
}

void compareFrames(char *path, char *frame, ReferenceFrame ref)
{
  FILE *frameFile;
  char *auxStr;
  int i, j, k, ct = 2;
  int aux = 0, px, pxRef;
  float bigger;

  int *frames = malloc(sizeof(int));
  int ctFrames = 0;
  char *resultStr, *auxResultStr;
  FILE *result;

  result = fopen("result.txt", "wb");

  do
  {
    auxStr = parser(ct);
    frameFile = openFile(path, frame, auxStr, 1);

    if (frameFile == NULL)
      break;

    for (i = 0; i < ref.width; i++)
      for (j = ref.initialHeight; j < ref.height; j++)
        for (k = 0; k < 3; k++)
        {
          px = fgetc(frameFile);
          pxRef = *(ref.rgb + i * ref.height * 3 + j * 3 + k);
          aux += abs(pxRef - px);
        }

    frames = realloc(frames, sizeof(int) * (ctFrames + 1));
    frames[ctFrames] = aux;
    ctFrames++;

    if (bigger < aux)
      bigger = aux;

    ct++;
    aux = 0;
    fclose(frameFile);
  } while (frameFile != NULL);

  bigger /= 2;

  resultStr = malloc((sizeof(char) * (22 + (7 * ctFrames) + ctFrames)));
  auxResultStr = malloc(sizeof(char) * ctFrames);

  strcpy(resultStr, "HOUVE MOVIMENTAÇÃO NO: \n");

  for (i = 0; i < ctFrames; i++)
  {
    if (frames[i] > bigger)
    {

      sprintf(auxResultStr, "%d", i + 2);
      strcat(resultStr, "Frame ");
      strcat(resultStr, auxResultStr);
      strcat(resultStr, "\n");
    }
  }

  free(frames);
  free(auxResultStr);
  free(auxStr);

  fputs(resultStr, result);
}

char *parser(int ct)
{
  char *str = malloc(sizeof(char) * MAX_SIZE_OF_FILE);
  char *parsed = malloc(sizeof(char) * MAX_SIZE_OF_FILE);

  if (ct < 10)
  {
    strcpy(str, "00");
    sprintf(parsed, "%d", ct);
    strcat(str, parsed);
  } // 0 - 9
  else if (ct >= 10 && ct < 100)
  {
    strcpy(str, "0");
    sprintf(parsed, "%d", ct);
    strcat(str, parsed);
  } // 10 - 99
  else
  {
    sprintf(parsed, "%d", ct);
    strcpy(str, parsed); // >= 100
  }

  free(parsed);

  return str;
}
