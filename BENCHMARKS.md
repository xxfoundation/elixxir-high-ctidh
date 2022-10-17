These very basic benchmarks are single core measurements and the CPUs are
little endian unless otherwise noted.

x86_64: i7-9750H
Benchmark for 64bit, x86_64 (original artisanal assembly)
511    Secret key generation time: 0.07912835106253624
511    Public key generation time: 0.07345771789550781
511    DH computation time: 0.07837618282064795
512    Secret key generation time: 0.062018395867198706
512    Public key generation time: 0.10401968518272042
512    DH computation time: 0.1051489058881998
1024   Secret key generation time: 0.1484885779209435
1024   Public key generation time: 0.39376416616141796
1024   DH computation time: 0.41494680382311344
2048   Secret key generation time: 0.05176710616797209
2048   Public key generation time: 1.7546545341610909
2048   DH computation time: 1.6683568777516484

x86_64: i7-9750H
Benchmark for 64bit, x86_64 (portable)
511    Secret key generation time: 0.09433999704197049
511    Public key generation time: 0.3587010409682989
511    DH computation time: 0.37568861711770296
512    Secret key generation time: 0.08370253583416343
512    Public key generation time: 0.5310733686201274
512    DH computation time: 0.5117835691198707
1024   Secret key generation time: 0.18872290989384055
1024   Public key generation time: 1.9766593631356955
1024   DH computation time: 1.8447958771139383
2048   Secret key generation time: 0.05854476289823651
2048   Public key generation time: 8.456408716738224
2048   DH computation time: 9.807465485762805

aarch64: Apple Mac Mini M1 (16K PAGESIZE)
Benchmark for 64bit, aarch64 (portable)
511    Secret key generation time: 0.06920927297323942
511    Public key generation time: 2.8887480589328334
511    DH computation time: 2.7935281139798462
512    Secret key generation time: 0.04019006493035704
512    Public key generation time: 0.21411600697319955
512    DH computation time: 0.21179521700832993
1024   Secret key generation time: 0.07814617396797985
1024   Public key generation time: 0.8357709189876914
1024   DH computation time: 0.832936072954908
2048   Secret key generation time: 0.028704492026008666
2048   Public key generation time: 4.827510087983683
2048   DH computation time: 4.669263796065934

aarch64: Cortex-A72, Raspberry Pi 4
Benchmark for 64bit, aarch64 (portable)
511    Secret key generation time: 0.5839935069961939
511    Public key generation time: 1.0675958649953827
511    DH computation time: 1.0644942259968957
512    Secret key generation time: 0.4719551970047178
512    Public key generation time: 1.495942533001653
512    DH computation time: 1.4920416910026688
1024   Secret key generation time: 1.059025554000982
1024   Public key generation time: 5.774300101998961
1024   DH computation time: 5.687486267997883
2048   Secret key generation time: 0.362219984002877
2048   Public key generation time: 37.726456261996645
2048   DH computation time: 36.866931747994386

ppc64: IBM POWER8 2.1 (pvr 004b 0201)
Benchmark for 64bit, ppc64
511    Secret key generation time: 0.1259909909858834
511    Public key generation time: 1.0422231259872206
511    DH computation time: 1.0447983450139873
512    Secret key generation time: 0.10751945900847204
512    Public key generation time: 1.4457009100005962
512    DH computation time: 1.4654621510126162
1024   Secret key generation time: 0.21632176500861533
1024   Public key generation time: 6.1101271040097345
1024   DH computation time: 6.015204277995508
2048   Secret key generation time: 0.07958257099380717
2048   Public key generation time: 38.191210492979735
2048   DH computation time: 35.99193976901006

ppc64le: POWER9 2.3 (pvr 004e 1203)
Benchmark for 64bit, ppc64le (portable)
511    Secret key generation time: 0.253029833547771
511    Public key generation time: 1.6347541995346546
511    DH computation time: 1.623527878895402
512    Secret key generation time: 0.22355646174401045
512    Public key generation time: 2.7481743413954973
512    DH computation time: 2.692152981646359
1024   Secret key generation time: 0.45237629767507315
1024   Public key generation time: 13.71284621488303
1024   DH computation time: 14.431835540570319
2048   Secret key generation time: 0.16986073646694422
2048   Public key generation time: 77.04607992526144
2048   DH computation time: 80.7134891403839

riscv64: JH7100 StarFive VisionFive v1
Benchmark for 64bit, riscv64 (portable)
511    Secret key generation time: 0.46746052568778396
511    Public key generation time: 1.5929640871472657
511    DH computation time: 1.6213686568662524
512    Secret key generation time: 0.42775739822536707
512    Public key generation time: 2.252150692977011
512    DH computation time: 2.2335860659368336
1024   Secret key generation time: 0.7870229231193662
1024   Public key generation time: 11.77524370374158
1024   DH computation time: 11.524985264986753
2048   Secret key generation time: 0.2966052610427141
2048   Public key generation time: 90.458395044785
2048   DH computation time: 93.26358337700367

loongarch64: Loongson-3A5000
Benchmark for 64bit, loongarch64 (portable)
511    Secret key generation time: 0.09855638997396454
511    Public key generation time: 5.113879900018219
511    DH computation time: 5.208397789974697
512    Secret key generation time: 0.08610675000818446
512    Public key generation time: 0.461977900005877
512    DH computation time: 0.45905876997858286
1024   Secret key generation time: 0.18068269995274022
1024   Public key generation time: 2.0299487999873236
1024   DH computation time: 2.029799399955664
2048   Secret key generation time: 0.06597716000396758
2048   Public key generation time: 21.42989051999757
2048   DH computation time: 21.416624400008004

s390x: IBM/S390 (5200MHz)
Benchmark for 64bit, s390x
511    Secret key generation time: 0.0638874470000701
511    Public key generation time: 1.3802173300000504
511    DH computation time: 1.3901836210000056
512    Secret key generation time: 0.06964657599996826
512    Public key generation time: 1.9471757379999417
512    DH computation time: 1.9461746120000498
1024   Secret key generation time: 0.10577733000013723
1024   Public key generation time: 7.341260884000121
1024   DH computation time: 7.321650286000022
2048   Secret key generation time: 0.04263353700002881
2048   Public key generation time: 32.35262795400013
2048   DH computation time: 32.35303378599997

sparc64: UltraSparc T3 (Niagara3) (Big Endian)
Benchmark for 64bit, sparc64
511    Secret key generation time: 0.6707389440853149
511    Public key generation time: 12.586691942997277
511    DH computation time: 12.754457036964595
512    Secret key generation time: 0.5827623668592423
512    Public key generation time: 17.354899911908433
512    DH computation time: 17.245990554103628
1024   Secret key generation time: 1.1556673340965062
1024   Public key generation time: 79.3382897770498
1024   DH computation time: 76.574702559039
2048   Secret key generation time: 0.4302366520278156
2048   Public key generation time: 338.6452274580952
2048   DH computation time: 359.59035670082085

sparc64: UltraSparc T5 (Niagara5) (Big Endian)
Benchmark for 64bit, sparc64
511    Secret key generation time: 0.2089481099974364
511    Public key generation time: 1.369832609896548
511    DH computation time: 1.357371918973513
512    Secret key generation time: 0.16970546904485673
512    Public key generation time: 1.8718948169844225
512    DH computation time: 1.8389898089226335
1024   Secret key generation time: 0.3714255370432511
1024   Public key generation time: 7.547536595026031
1024   DH computation time: 7.770486026071012
2048   Secret key generation time: 0.1368694039992988
2048   Public key generation time: 50.581388552091084
2048   DH computation time: 50.31374777609017

riscv64: HiFive Unmatched; SiFive Freedom U740
Benchmark for 64bit, riscv64
511    Secret key generation time: 0.427932433784008
511    Public key generation time: 2.0187510419636965
511    DH computation time: 2.061710085719824
512    Secret key generation time: 0.37768438179045916
512    Public key generation time: 2.789009821601212
512    DH computation time: 2.7478867806494236
1024   Secret key generation time: 0.7678517773747444
1024   Public key generation time: 11.076364214532077
1024   DH computation time: 11.20567935705185
2048   Secret key generation time: 0.2868962902575731
2048   Public key generation time: 144.48563995119184
2048   DH computation time: 135.50788285490125
