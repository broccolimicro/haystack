.model chip
.inputs a.r b.a call.r
.outputs a.a b.r call.a
.dummy T0 T1 T3 T4 T5 T6 T8 T9 T10 T12 T14 T15 T16 T18 T19 T20 T21 T22 T23 T25 T26
.graph
T9 S2
a.r- S2
T5 S3
a.r+ S3
T19 S4
b.a- S4
T15 S5
b.a+ S5
T22 S10
call.r- S10
T0 S11
call.r+ S11
T26 S12
T0 S13
T1 S14
call.a+ S15
T3 S16
T20 S16
T4 S17
T5 S18
T6 S19
a.a+ S20
T8 S21
T9 S22
T10 S23
a.a- S24
T12 S25
b.r+ S26
T14 S27
T15 S28
T16 S29
b.r- S30
T18 S31
T19 S32
T21 S33
T22 S34
T23 S35
call.a- S36
T25 S37
S2 T9
S2 a.r+
S3 T5
S3 a.r-
S4 T19
S4 b.a+
S5 T15
S5 b.a-
S10 T22
S10 call.r+
S11 T0
S11 call.r-
S12 T0
S13 T1
S14 call.a+
S15 T3
S16 T4
S16 T21
S17 T5
S18 T6
S19 a.a+
S20 T8
S21 T9
S22 T10
S23 a.a-
S24 T12
S25 b.r+
S26 T14
S27 T15
S28 T16
S29 b.r-
S30 T18
S31 T19
S32 T20
S33 T22
S34 T23
S35 call.a-
S36 T25
S37 T26
.marking {S2 S4 S10 S12}
.end
