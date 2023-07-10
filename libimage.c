/* Paul Gaborit - Ecole des mines d'Albi-Carmaux - 1997-2020 */
#include "libimage.h"
#include <string.h>
#include <math.h>

/* Definition de deux 'couleur' de couleurs particulieres */
couleur noir = { 0, 0, 0};
couleur blanc = { 255, 255, 255};

/* Fonction d'allocation d'une image d'une taille donnee */
image nouvelle_image(int largeur, int hauteur)
{
  image im;

  im.largeur = largeur;
  im.hauteur = hauteur;
  im.tabPoints = (couleur *) malloc( sizeof(couleur) * largeur * hauteur);
  if (im.tabPoints == NULL) {
    fprintf(stderr,
            "(%s) pas assez de memoire ! (image: %dx%d)\n",
	    __func__,
            largeur, hauteur);
    exit(1);
  }
  return im;
}

/* Fonction de destruction d'une image precedemment allouee */
void detruire_image(image im)
{
  free(im.tabPoints);
}

/* Fonction "lire_couleur" retourne la couleur du point de
   coordonnées x, y de l'image im */
couleur lire_couleur(image im, int x, int y)
{
  if (   (x >= 0)
         && (x < im.largeur)
         && (y >= 0)
         && (y < im.hauteur) ) {
    return im.tabPoints[ x + (y * im.largeur) ];
  } else {
    fprintf(stderr,
            "(%s) point (%d, %d) hors image (image: %dx%d)!\n",
	    __func__,
            x, y,
            im.largeur, im.hauteur);
    exit(1);
  }
}

/* La fonction "change_couleur" donne la couleur "nouv" au point de
   coordonnees x, y de l'image "im" */
void change_couleur(image im, int x, int y, couleur nouv)
{
  if ((x >= 0)
      && (x < im.largeur)
      && (y >= 0)
      && (y < im.hauteur))
    {
      if (nouv.rouge < 0) {
        nouv.rouge = 0;
        fprintf(stderr, "(%s) rouge < 0 !\n", __func__);
      } else if (nouv.rouge > 255) {
        nouv.rouge = 255;
        fprintf(stderr, "(%s) rouge > 255 !\n", __func__);
      }
      if (nouv.vert < 0) {
        nouv.vert = 0;
        fprintf(stderr, "(%s) vert < 0 !\n", __func__);
      } else if (nouv.vert > 255) {
        nouv.vert = 255;
        fprintf(stderr, "(%s) vert > 255 !\n", __func__);
      }
      if (nouv.bleu < 0) {
        nouv.bleu = 0;
        fprintf(stderr, "(%s) bleu < 0 !\n", __func__);
      } else if (nouv.bleu > 255) {
        nouv.bleu = 255;
        fprintf(stderr, "(%s) bleu > 255 !\n", __func__);
      }
      im.tabPoints[ x + (y * im.largeur) ] = nouv;
    } else {
      fprintf(stderr, "(%s) point (%d, %d) hors image (image: %dx%d)!\n",
	      __func__,
              x, y,
	      im.largeur, im.hauteur);
    }
}

/* La fonction "lire_image" lit, dans le fichier de nom
   "nom", une image (stockee au format PPM) */
image lire_image(char * nom)
{
  FILE          *fich;
  char           type[255];
  int            x, y;
  int            largeur, hauteur;
  couleur        coul;
  unsigned char  val;
  image          im;
  int            profondeur;

  /* ouverture du fichier */
  fich = fopen(nom, "rb");
  if (fich == NULL) {
    fprintf(stderr, "(%s) fichier %s non lisible !\n", __func__, nom);
    exit(1);
  }

  /* lecture du type */
  int res = fscanf(fich, "%254s", type);
  if (res != 1 || strcmp(type, "P6") != 0) {
    fprintf(stderr, "(%s) fichier '%s' pas au format ppm (P6)\n", __func__, nom);
    exit(1);
  }

  /* lecture de la fin de ligne */
  while(getc(fich) != '\n') {}

  /* lecture des commentaires */
  {
    char next_car = getc(fich);
    while(next_car == '#') {
      while(getc(fich) != '\n');
      next_car = getc(fich);
    }
    ungetc(next_car, fich);
  }

  /* lecture de la taille de l'image */
  res = fscanf(fich, "%d %d", &largeur, &hauteur);
  /* lecture de la profondeur */
  res += fscanf(fich, "%d", &profondeur);
  /* lecture du dernier retour a la ligne */
  res += fscanf(fich, "%c", &val);

  if (res != 4) {
    fprintf(stderr, "(%s) fichier '%s', mauvais entete PPM\n", __func__, nom);
    exit(1);
  }
  
  im = nouvelle_image(largeur, hauteur);

  for (y = 0; y < hauteur; y++) {
    for (x = 0; x < largeur; x++) {
      res = fscanf(fich, "%c", &val);
      coul.rouge = (int)(val * 256) / (profondeur + 1);
      res += fscanf(fich, "%c", &val);
      coul.vert = (int) (val * 256) / (profondeur + 1);
      res += fscanf(fich, "%c", &val);
      coul.bleu = (int) (val * 256) / (profondeur + 1);
      change_couleur(im, x, y, coul);
      if (res != 3) {
	fprintf(stderr, "(%s) dans fichier '%s', couleur bizarre\n", __func__, nom);
	exit(1);
      }
    }
  }
  fclose(fich);

  fprintf(stderr,
          "(%s) lecture du  fichier '%s' qui contient une image (%dx%d)\n",
	  __func__,
          nom,
          im.largeur, im.hauteur);

  return im;
}

/* La fonction "ecrire_image" enregistre l'image "im" dans un fichier
   de nom "nom" (au format PPM) */
void ecrire_image(image im, char * nom)
{
  FILE    *fich;
  int     x,y;
  couleur coul;

  fich = fopen(nom, "wb");
  if (fich == NULL) {
    fprintf(stderr, "(%s) ecriture du fichier '%s' impossible\n", __func__, nom);
    exit(1);
  }
  fprintf(fich, "P6\n");
  fprintf(fich, "# Cree par libimage.a - (C) 1997 P.Gaborit - EMAC\n");
  fprintf(fich, "%d %d\n", im.largeur, im.hauteur);
  fprintf(fich, "255\n");

  for (y = 0; y < im.hauteur; y++) {
    for (x = 0; x < im.largeur; x++) {
      coul = lire_couleur(im, x, y);
      fprintf(fich, "%c", coul.rouge);
      fprintf(fich, "%c", coul.vert);
      fprintf(fich, "%c", coul.bleu);
    }
  }
  fclose(fich);

  fprintf(stderr,
          "(%s) ecriture d'une image %dx%d dans le fichier '%s'\n",
	  __func__,
          im.largeur, im.hauteur, nom);
}


/* transformation d'une couleur HSV ([0-359], [0-1], [0-1]) en une
   couleur RGB (3 x [0-255]). Derived from C/C++ Macro HSV to RGB by
   T. Nathan Mundhenk mundhenk@usc.edu */

void hsv_to_rgb(double h, double s, double v,
		int * rip, int * gip, int * bip)
{
  double r, g, b;
  
  if (v == 0) {
    r = 0; g = 0; b = 0;
  } else if (s == 0) {
    r = v; g = v; b = v;
  } else {
    const double hf = h / 60.0;
    const int    i  = hf < (int) hf ? (int) hf - 1 : (int) hf;
    const double f  = hf - i;
    const double pv  = v * (1 - s);
    const double qv  = v * (1 - s * f);
    const double tv  = v * (1 - s * (1 - f));
    switch (i) {
    case 0:  r = v;  g = tv; b = pv; break;
    case 1:  r = qv; g = v;  b = pv; break;
    case 2:  r = pv; g = v;  b = tv; break;
    case 3:  r = pv; g = qv; b = v;  break;
    case 4:  r = tv; g = pv; b = v;  break;
    case 5:  r = v;  g = pv; b = qv; break;
    case 6:  r = v;  g = tv; b = pv; break;
    case -1: r = v;  g = pv; b = qv; break;
    default:
      fprintf(stderr, "(%s) i value error in color conversion, value is %d", __func__, i);
      exit(1);
      break;
    }
  }
  *rip = r*255;
  if (r*255-*rip >= 0.5) {*rip+=1;}
  *gip = g*255;
  if (g*255-*gip >= 0.5) {*gip+=1;}
  *bip = b*255;
  if (b*255-*bip >= 0.5) {*bip+=1;}
}

/* transformation d'une couleur RGB (3 x [0-255]) en une couleur HSV
 ([0-359], [0-1], [0-1]) Derived from
 <https://stackoverflow.com/a/6930407/3384763>
 */

void rgb_to_hsv(int r, int g, int b, double * hp, double * sp, double * vp)
{
  double rn = r/255.0;
  double gn = g/255.0;
  double bn = b/255.0;

  //hsv         out;
  
  double      min, max, delta;
  
  min = rn < gn ? rn : gn;
  min = min  < bn ? min  : bn;
  
  max = rn > gn ? rn : gn;
  max = max  > bn ? max  : bn;
  
  *vp = max;                                // v
  delta = max - min;
  if (delta < 0.00001)
    {
      *sp = 0;
      *hp = 0; // undefined, maybe nan?
      return;
    }
  if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
    *sp = (delta / max);                  // s
  } else {
    // if max is 0, then r = g = b = 0              
    // s = 0, h is undefined
    *sp = 0.0;
    *hp = NAN;                            // its now undefined
    return;
  }
  if( rn >= max )                           // > is bogus, just keeps compilor happy
    *hp = ( gn - bn ) / delta;        // between yellow & magenta
  else
    if( gn >= max )
      *hp = 2.0 + ( bn - rn ) / delta;  // between cyan & yellow
    else
      *hp = 4.0 + ( rn - gn ) / delta;  // between magenta & cyan
  
  *hp *= 60.0;                              // degrees
  
  if( *hp < 0.0 )
    *hp += 360.0;  
}

