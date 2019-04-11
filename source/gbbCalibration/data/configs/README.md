## config explanation:

In run_GbbTupleAna_Calib.cfg

Option 

BTagWP : Btaggin working points setting.
* choose and set the tagger type and mixing fraction f and efficency eff
* use = and | to separate like examples below:

1. For XbbScore Tagger The config string is 
`tagger=XbbScore|f=<0.2>|eff=<60>` 
* f = 0.2 or 0
* eff = 50 or 60 or 70

2. For hybrid XbbScore use: `tagger=XbbScoreHybrid|eff=60|f=0.2`
    * only eff=60 and f=0.2 or 0 is supported 

3. For MV2c10 Tagger config String is :`tagger=MV2c10|eff=<60>` 
    * eff = 60, 70, 77, or 85

4. For MV2c10R20.7 
`tagger=MV2c10R20.7|eff=70` is supported

5. TODO: not yet implemented 
"tagger=<>|cutValue=<>"
