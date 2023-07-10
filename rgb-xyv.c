#include "libimage.h"
#include <math.h>


// nouveaux types
typedef couleur couleur_rgb;
typedef struct {
  double h;
  double s;
  double v;
} couleur_hsv;
typedef struct {
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


// nouvelles fonctions de conversions RGB <=> HSV
couleur_hsv rgb2hsv(couleur_rgb c_rgb) {
  couleur_hsv c_hsv;
  rgb_to_hsv(c_rgb.rouge, c_rgb.vert, c_rgb.bleu, &c_hsv.h, &c_hsv.s, &c_hsv.v);
  return c_hsv;
}

couleur_rgb hsv2rgb(couleur_hsv c_hsv) {
  couleur_rgb c_rgb;
  hsv_to_rgb(c_hsv.h, c_hsv.s, c_hsv.v, &c_rgb.rouge, &c_rgb.vert, &c_rgb.bleu);
  return c_rgb;
}


// nouvelles fonctions de conversions XYV <=> HSV
couleur_xyv hsv2xyv(couleur_hsv c_hsv){
  couleur_xyv c_xyv;
  c_xyv.x=c_hsv.s*cos(c_hsv.h* M_PI / 180.0);
  c_xyv.y=c_hsv.s*sin(c_hsv.h* M_PI / 180.0);
  c_xyv.v=c_hsv.v;
  return c_xyv;
}

couleur_hsv xyv2hsv(couleur_xyv c_xyv) {
  couleur_hsv c_hsv;  
  c_hsv.s=sqrt(c_xyv.x*c_xyv.x+c_xyv.y*c_xyv.y);

  c_hsv.h= atan2(c_xyv.y,c_xyv.x)*180.0/M_PI; 
  if(c_hsv.h<0) c_hsv.h+=360.0;
  c_hsv.v=c_xyv.v;
  return c_hsv;
}


// nouvelles fonctions de conversions XYV <=> RGB
couleur_xyv rgb2xyv(couleur_rgb c_rgb){
  couleur_hsv c_hsv=rgb2hsv(c_rgb);
  couleur_xyv c_xyv=hsv2xyv(c_hsv);
  return c_xyv;
}

couleur_rgb xyv2rgb(couleur_xyv c_xyv) {
  couleur_hsv c_hsv=xyv2hsv(c_xyv);
  couleur_rgb c_rgb= hsv2rgb(c_hsv);
  return c_rgb;
}


// tests unitaires et test de stabilité des conversions
int main() {
  couleur_rgb c_rgb;
  int nb_conversions_ok = 0;
  
  for (c_rgb.rouge = 0; c_rgb.rouge < 256; c_rgb.rouge++) {
    for (c_rgb.vert = 0; c_rgb.vert < 256; c_rgb.vert++) {
      for (c_rgb.bleu = 0; c_rgb.bleu < 256; c_rgb.bleu++) {
	couleur_xyv c_xyv;
	couleur_rgb c_rgb_verif;

	// conversion aller-retour entre rgb et xyv (pour tester les nouvelles fonctions)
	c_xyv = rgb2xyv(c_rgb);
	c_rgb_verif = xyv2rgb(c_xyv);

	// vérification de la stabilité
	if (c_rgb_verif.rouge != c_rgb.rouge
	    || c_rgb_verif.vert != c_rgb.vert
	    || c_rgb_verif.bleu != c_rgb.bleu) {
	  printf("Conversion instable: (%d,%d,%d) => (%d,%d,%d)\n",
		 c_rgb.rouge, c_rgb.vert, c_rgb.bleu,
		 c_rgb_verif.rouge, c_rgb_verif.vert, c_rgb_verif.bleu);
	} else {
	  nb_conversions_ok++;
	}
      }
    }
  }
  
  printf("Nb de conversions stables: %d\n", nb_conversions_ok);
  if (nb_conversions_ok == 256 * 256 * 256) {
    printf("Conversions stables pour toutes les couleurs RVB\n");
  }
  return 0;
}
