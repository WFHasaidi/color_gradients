#include <stdlib.h>
#include <math.h>
#include "libimage.h"

// nouveaux types
typedef couleur couleur_rgb;
typedef struct
{
  double h;
  double s;
  double v;
} couleur_hsv;


typedef struct
{
  double x;
  double y;
  double v;
} couleur_xyv;

// prototypes des fonctions
couleur_hsv rgb2hsv(couleur_rgb c_rgb);
couleur_rgb hsv2rgb(couleur_hsv c_hsv);
couleur_xyv hsv2xyv(couleur_hsv c_hsv);
couleur_hsv xyv2hsv(couleur_xyv c_xyv);
couleur_xyv rgb2xyv(couleur_rgb c_rgb);
couleur_rgb xyv2rgb(couleur_xyv c_xyv);
couleur_rgb interpolation_rgb(double poids, couleur_rgb c1, couleur_rgb c2);
couleur_xyv interpolation_xyv(double poids, couleur_xyv c1, couleur_xyv c2);
void gradient_rgb(image im, couleur_rgb c_left_haut, couleur_rgb c_right_haut, couleur_rgb c_left_bas, couleur_rgb c_right_bas);
void gradient_xyv(image im, couleur_xyv c_left_haut, couleur_xyv c_right_haut, couleur_xyv c_left_bas, couleur_xyv c_right_bas);
void usage(char *nom_programme, char *message);
couleur_rgb argv_vers_couleur_rgb(char *nom_programme, char *argument);

// nouvelles fonctions de conversions RGB <=> HSV
couleur_hsv rgb2hsv(couleur_rgb c_rgb)
{
  couleur_hsv c_hsv;
  rgb_to_hsv(c_rgb.rouge, c_rgb.vert, c_rgb.bleu, &c_hsv.h, &c_hsv.s, &c_hsv.v);
  return c_hsv;
}

couleur_rgb hsv2rgb(couleur_hsv c_hsv)
{
  couleur_rgb c_rgb;
  hsv_to_rgb(c_hsv.h, c_hsv.s, c_hsv.v, &c_rgb.rouge, &c_rgb.vert, &c_rgb.bleu);
  return c_rgb;
}

// nouvelles fonctions de conversions XYV <=> HSV
couleur_xyv hsv2xyv(couleur_hsv c_hsv)
{
  couleur_xyv c_xyv;
  c_xyv.x = c_hsv.s * cos(c_hsv.h * M_PI / 180.0);
  c_xyv.y = c_hsv.s * sin(c_hsv.h * M_PI / 180.0);
  c_xyv.v = c_hsv.v;
  return c_xyv;
}

couleur_hsv xyv2hsv(couleur_xyv c_xyv)
{
  couleur_hsv c_hsv;
  c_hsv.s = sqrt(c_xyv.x * c_xyv.x + c_xyv.y * c_xyv.y);
  c_hsv.h = atan2(c_xyv.y, c_xyv.x) * 180.0 / M_PI;
  if (c_hsv.h < 0)
    c_hsv.h += 360.0;
  c_hsv.v = c_xyv.v;
  return c_hsv;
}

// nouvelles fonctions de conversions XYV <=> RGB
couleur_xyv rgb2xyv(couleur_rgb c_rgb)
{
  couleur_hsv c_hsv = rgb2hsv(c_rgb);
  couleur_xyv c_xyv = hsv2xyv(c_hsv);
  return c_xyv;
}

couleur_rgb xyv2rgb(couleur_xyv c_xyv)
{
  couleur_hsv c_hsv = xyv2hsv(c_xyv);
  couleur_rgb c_rgb = hsv2rgb(c_hsv);
  return c_rgb;
}

// fonction d'interpolation entre deux couleurs dans l'espace RGB
couleur_rgb interpolation_rgb(double poids, couleur_rgb c1, couleur_rgb c2)
{
  couleur_rgb res;

  res.rouge = (1 - poids) * c1.rouge + poids * c2.rouge;
  res.vert = (1 - poids) * c1.vert + poids * c2.vert;
  res.bleu = (1 - poids) * c1.bleu + poids * c2.bleu;

  return res;
}

// fonction d'interpolation entre deux couleurs dans l'espace XYV
couleur_xyv interpolation_xyv(double poids, couleur_xyv c1, couleur_xyv c2)
{
  couleur_xyv res;

  res.x = (1 - poids) * c1.x + poids * c2.x;
  res.y = (1 - poids) * c1.y + poids * c2.y;
  res.v = (1 - poids) * c1.v + poids * c2.v;

  return res;
}

// remplissage d'une image par un dégradé RGB horizontal
void gradient_rgb(image im, couleur_rgb c_rgb_gauche_haut,couleur_rgb c_rgb_droite_haut,couleur_rgb c_rgb_gauche_bas,couleur_rgb c_rgb_droite_bas)
{
  int x, y;
  for (x = 0; x < im.largeur; x++)
  {
    couleur_rgb c1, c2, c3;

    double proportion_x = x * 1.0 / (im.largeur - 1);

    c1 = interpolation_rgb(proportion_x,  c_rgb_gauche_haut, c_rgb_droite_haut );
    c2 = interpolation_rgb(proportion_x,  c_rgb_gauche_bas  ,c_rgb_droite_bas );

    for (y = 0; y < im.hauteur; y++)
    {
      double proportion_y = y * 1.0 / (im.hauteur - 1);
      c3 = interpolation_rgb(proportion_y, c1,c2);
      change_couleur(im, x, y, c3);
    }
  }
}

// remplissage d'une image par un dégradé RGB horizontal
void gradient_xyv(image im, couleur_xyv c_xyv_gauche_haut,couleur_xyv c_xyv_droite_haut,couleur_xyv c_xyv_gauche_bas,couleur_xyv c_xyv_droite_bas)
{ 

  int x, y;
  for (x = 0; x < im.largeur; x++)
  {
    couleur_xyv c_xyv1,c_xyv2,c_xyv3;
    couleur_rgb c_rgb;

    double proportion_x = x * 1.0 / (im.largeur - 1);

    c_xyv1 = interpolation_xyv(proportion_x, c_xyv_gauche_haut, c_xyv_droite_haut);
    c_xyv2 = interpolation_xyv(proportion_x, c_xyv_gauche_bas, c_xyv_droite_bas);

    for (y = 0; y < im.hauteur; y++)
    {
      double proportion_y = y * 1.0 / (im.hauteur - 1);
      c_xyv3 = interpolation_xyv(proportion_y, c_xyv1, c_xyv2);
      c_rgb = xyv2rgb(c_xyv3);
      change_couleur(im, x, y, c_rgb);
    }
  }

}

// description de l'usage du programme
void usage(char *nom_programme, char *message)
{
  fprintf(stderr, "Erreur: %s\n", message);
  fprintf(stderr, "\n");
  fprintf(stderr, "Usage: %s result.ppm LxH r1,v1,b1 r2,v2,b2 r3,v3,b3 r4,v4,b4 algo\n", nom_programme);
  fprintf(stderr, "\n");
  fprintf(stderr, "   result.ppm: fichier image a produire\n");
  fprintf(stderr, "   L, H: largeur et hauteur de l'image (entiers positifs)\n");
  fprintf(stderr, "   r,v,b: composantes rouge, verte et bleue (entiers 0-255)\n");
  fprintf(stderr, "   algo:  0 pour un dégradé dans l’espace RGB ou 1 pour un dégradé l’espace XYV\n");  
  exit(1);
}

// fonction utilitaire d'extraction d'une couleur RGB d'un argument
couleur_rgb argv_vers_couleur_rgb(char *nom_programme, char *argument)
{
  couleur_rgb c;

  int res_lecture = sscanf(argument, "%d,%d,%d", &c.rouge, &c.vert, &c.bleu);
  if (res_lecture != 3)
  {
    usage(nom_programme, "composantes r,v,b illisibles");
  }
  if ((c.rouge < 0 || c.rouge > 255) || (c.vert < 0 || c.vert > 255) || (c.bleu < 0 || c.bleu > 255))
  {
    fprintf(stderr, "Erreur dans '%s'\n", argument);
    usage(nom_programme, "composantes r,v,b hors intervalle 0-255");
  }
  return c;
}

int main(int argc, char *argv[])
{
  int largeur, hauteur;
  couleur_rgb c_rgb_gauche_haut, c_rgb_droite_haut, c_rgb_gauche_bas, c_rgb_droite_bas;
  couleur_xyv c_xyv_gauche_haut, c_xyv_droite_haut, c_xyv_gauche_bas, c_xyv_droite_bas;
  int res_lecture, type_color;
  image im;

  // vérification du bon nombre d'arguments
  if (argc != 8)
  {
    usage(argv[0], "nombre d'arguments incorrect");
  }

  // extraction de la largeur et de la hauteur
  res_lecture = sscanf(argv[2], "%dx%d", &largeur, &hauteur);
  if (res_lecture != 2)
  {
    usage(argv[0], "valeurs entieres LxH illisibes");
  }
  if (largeur < 1 || hauteur < 1)
  {
    usage(argv[0], "L et H ne sont pas strictement positifs");
  }

  // fabrication de l'image
  im = nouvelle_image(largeur, hauteur);

  c_rgb_gauche_haut = argv_vers_couleur_rgb(argv[0], argv[3]);
  c_rgb_droite_haut = argv_vers_couleur_rgb(argv[0], argv[4]);

  c_rgb_droite_bas = argv_vers_couleur_rgb(argv[0], argv[5]);
  c_rgb_gauche_bas = argv_vers_couleur_rgb(argv[0], argv[6]);

  sscanf(argv[7], "%d", &type_color);
  if (type_color == 0)
  {
    // extraction des deux couleurs
    gradient_rgb(im, c_rgb_gauche_haut, c_rgb_droite_haut, c_rgb_gauche_bas, c_rgb_droite_bas);
  }
  else if (type_color == 1)
  {
    c_xyv_gauche_haut = rgb2xyv(c_rgb_gauche_haut);
    c_xyv_droite_haut = rgb2xyv(c_rgb_droite_haut);
    c_xyv_gauche_bas = rgb2xyv(c_rgb_gauche_bas);
    c_xyv_droite_bas = rgb2xyv(c_rgb_droite_bas);
    gradient_xyv(im, c_xyv_gauche_haut, c_xyv_droite_haut, c_xyv_gauche_bas, c_xyv_droite_bas);
  }

  // écriture de l'image finale
  ecrire_image(im, argv[1]);
  return 0;
}