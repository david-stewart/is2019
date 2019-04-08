# pi-plus test
#star-submit-template -template sums/m.xml -entities outname=pi_plus_test,dir=/gpfs/mnt/gpfs01/star/embed/embedding/production_pAu200_2015/Piplus_200_20180601/P16id.SL16d_embed/2015/158/
starver SL16d_embed
echo $STAR_VERSION

# pi-plus
star-submit-template -template sums/m.xml -entities outname=pi_plus_fullSample,dir=/gpfs/mnt/gpfs01/star/embed/embedding/production_pAu200_2015/Piplus_200_20180601/P16id.SL16d_embed/2015

# pi-minus 
star-submit-template -template sums/m.xml -entities outname=pi_minus_fullSample,dir=/gpfs/mnt/gpfs01/star/embed/embedding/production_pAu200_2015/Piminus_200_20180601/P16id.SL16d_embed/2015

# Kaon+ 
star-submit-template -template sums/m.xml -entities outname=Kplus_fullSample,dir=/gpfs/mnt/gpfs01/star/embed/embedding/production_pAu200_2015/Kplus_200_20180601/P16id.SL16d_embed/2015

# Kminus
star-submit-template -template sums/m.xml -entities outname=Kminus_fullSample,dir=/gpfs/mnt/gpfs01/star/embed/embedding/production_pAu200_2015/Kminus_200_20180601/P16id.SL16d_embed/2015

# proton
star-submit-template -template sums/m.xml -entities outname=proton_fullSample,dir=/gpfs/mnt/gpfs01/star/embed/embedding/production_pAu200_2015/Proton_200_20180601/P16id.SL16d_embed/2015

# antiproton
star-submit-template -template sums/m.xml -entities outname=proton_fullSample,dir=/gpfs/mnt/gpfs01/star/embed/embedding/production_pAu200_2015/AntiProton_200_20180601/P16id.SL16d_embed/2015
