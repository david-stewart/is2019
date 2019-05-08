phenix=../../data/phenix_pikp_data/piKp_phenix.root
star=../../data/star_pip_data/pip_STAR.root
to_file=StarPhenix_piKp_comp.root
rootcp   $phenix:p_0_100     $to_file:p_phenix
rootcp   $phenix:pbar_0_100  $to_file:pbar_phenix
rootcp   $phenix:pin_0_100   $to_file:pin_phenix
rootcp   $phenix:pi_0_100    $to_file:pi_phenix
rootcp   $star:p_MB          $to_file:p_star
rootcp   $star:pbar_MB       $to_file:pbar_star
rootcp   $star:pi_MB         $to_file:pi_star
rootcp   $star:pin_MB        $to_file:pin_star
rootcp   $star:pin_MB        $to_file:pin_star

rootcp   $phenix:K_0_100     $to_file:K_phenix
rootcp   $phenix:Kn_0_100    $to_file:Kn_phenix
