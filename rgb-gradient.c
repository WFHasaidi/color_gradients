#include <stdlib.h>
#include <math.h>
#include "libimage.h"

// nouveaux types
typedef couleur couleur_rgb;

typedef struct {
  double h;
  double s;
  double v;
} couleur_hsv;

// prototypes des fonctions
couleur_hsv rgb2hsv(couleur_rgb c_rgb);
couleur_rgb hsv2rgb(couleur_hsv c_hsv);
couleur_rgb interpolation_rgb(double poids, couleur_rgb c1, couleur_rgb c2);
void usage(char * nom_programme, char * message);
couleur_rgb argv_vers_couleur_rgb(char * nom_programme, char * argument);

// nouvelles fonctions de conversions RGB <=> HSV
couleur_hsv rgb2hsv(couleur_rgb c_rgb) {
  couleur_hsv c_hsv;
  rgb_to_hsv(c_rgb.rouge, c_rgb.vert, c_rgb.bleu, &c_hsv.h, &c_hsv.s, &c_hsv.v);
  return c_hsv;
}

couleur_rgb hsv2rgb(couleur_hsv c_hsv) {
  couleur c_rgb;
  hsv_to_rgb(c_hsv.h, c_hsv.s, c_hsv.v, &c_rgb.rouge, &c_rgb.vert, &c_rgb.bleu);
  return c_rgb;
}

// fonction d'interpolation entre deux couleurs dans l'espace RGB
couleur_rgb interpolation_rgb(double poids, couleur_rgb c1, couleur_rgb c2) {
  couleur res;

  res.rouge = (1 - poids) * c1.rouge + poids * c2.rouge;
  res.vert =  (1 - poids) * c1.vert + poids * c2.vert;
  res.bleu =  (1 - poids) * c1.bleu + poids * c2.bleu;
  
  return res;
}

// remplissage d'une image par un dégradé RGB horizontal
void gradient_rgb(image im, couleur_rgb c_left, couleur_rgb c_right) {
  int x, y;
  for (x = 0; x < im.largeur; x++) {
    couleur_rgb c;
    
    double proportion = x*1.0/(im.largeur-1);
    c = interpolation_rgb(proportion, c_left, c_right);
    
    for (y = 0; y < im.hauteur; y++) {
      change_couleur(im, x, y, c);
    }
  }
}

// description de l'usage du programme
void usage(char * nom_programme, char * message) {
  fprintf(stderr, "Erreur: %s\n", message);
  fprintf(stderr, "\n");
  fprintf(stderr, "Usage: %s result.ppm LxH r1,v1,b1 r2,v2,b2\n", nom_programme);
  fprintf(stderr, "\n");
  fprintf(stderr, "   result.ppm: fichier image a produire\n");
  fprintf(stderr, "   L, H: largeur et hauteur de l'image (entiers positifs)\n");
  fprintf(stderr, "   r,v,b: composantes rouge, verte et bleue (entiers 0-255)\n");
  exit(1);
}

// fonction utilitaire d'extraction d'une couleur RGB d'un argument
couleur_rgb argv_vers_couleur_rgb(char * nom_programme, char * argument) {
  couleur_rgb c;
  
  int res_lecture = sscanf(argument, "%d,%d,%d", &c.rouge, &c.vert, &c.bleu);
  if (res_lecture != 3) {
    usage(nom_programme, "composantes r,v,b illisibles");
  }
  if ((c.rouge < 0 || c.rouge > 255)
      || (c.vert < 0 || c.vert > 255)
      || (c.bleu < 0 || c.bleu > 255)) {
    fprintf(stderr, "Erreur dans '%s'\n", argument);
    usage(nom_programme, "composantes r,v,b hors intervalle 0-255");
  }
  return c;
}

int main(int argc, char *argv[]) {
  int largeur, hauteur;
  couleur_rgb c_gauche, c_droite;
  int res_lecture;
  image im;

  // vérification du bon nombre d'arguments
  if (argc != 5) {
    usage(argv[0], "nombre d'arguments incorrect");
  }

  // extraction de la largeur et de la hauteur
  res_lecture = sscanf(argv[2], "%dx%d", &largeur, &hauteur);
  if (res_lecture != 2) {
    usage(argv[0], "valeurs entieres LxH illisibes");
  }
  if (largeur < 1 || hauteur < 1) {
    usage(argv[0], "L et H ne sont pas strictement positifs");
  }

  // extraction des deux couleurs
  c_gauche = argv_vers_couleur_rgb(argv[0], argv[3]);
  c_droite = argv_vers_couleur_rgb(argv[0], argv[4]);

  // fabrication de l'image
  im = nouvelle_image(largeur, hauteur);
  gradient_rgb(im, c_gauche, c_droite);

  // écriture de l'image finale
  ecrire_image(im, argv[1]);
  return 0;
}
