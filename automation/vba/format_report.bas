Attribute VB_Name = "FormatTrackGuardReport"

Sub FormatTrackGuardReport()

    Dim ws As Worksheet
    Dim lastRow As Long
    Dim i As Long
    Dim healthyCount As Long
    Dim faultedCount As Long

    Set ws = ActiveSheet
    lastRow = ws.Cells(ws.Rows.Count, 1).End(xlUp).Row

    healthyCount = 0
    faultedCount = 0

    ' Format header row
    ws.Rows(1).Font.Bold = True
    ws.Rows(1).Interior.Color = RGB(200, 200, 200)

    ' Auto-fit data columns
    ws.Columns("A:D").AutoFit

    ' Color rows based on fault status
    For i = 2 To lastRow

        If ws.Cells(i, 4).Value = "NONE" Then

            healthyCount = healthyCount + 1
            ws.Rows(i).Interior.Color = RGB(204, 255, 204)

        Else

            faultedCount = faultedCount + 1
            ws.Rows(i).Interior.Color = RGB(255, 199, 206)
            ws.Cells(i, 4).Font.Bold = True

        End If

    Next i

    ' Summary section
    ws.Range("F1").Value = "TrackGuard Summary"
    ws.Range("F1").Font.Bold = True

    ws.Range("F2").Value = "Total Devices"
    ws.Range("G2").Value = lastRow - 1

    ws.Range("F3").Value = "Healthy Devices"
    ws.Range("G3").Value = healthyCount

    ws.Range("F4").Value = "Faulted Devices"
    ws.Range("G4").Value = faultedCount

    ws.Columns("F:G").AutoFit

    MsgBox "TrackGuard report formatted successfully."

End Sub