/*
  Biometric module library fucntions
  1. Enroll the biometric fingerprint
  2. Delte the biometric fingerprint
  3. Match the biometric fingerperint
  4. Emptry the biometric module 
  5. chekc the finger print.  

*/


///////// finger print enrollment /////////////


  uint8_t getFingerprintEnroll() {

    int p = -1;
    Serial.print("Waiting for valid finger to enroll as #");
    Serial.println(id);
    while (p != FINGERPRINT_OK) {
      p = finger.getImage();
      switch (p) {
        case FINGERPRINT_OK:
          Serial.println("Image taken");
          break;
        case FINGERPRINT_NOFINGER:
          Serial.print(".");
          break;
        case FINGERPRINT_PACKETRECIEVEERR:
          Serial.println("Communication error");
          break;
        case FINGERPRINT_IMAGEFAIL:
          Serial.println("Imaging error");
          break;
        default:
          Serial.println("Unknown error");
          break;
      }
    }

    // OK success!

    p = finger.image2Tz(1);
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image converted");
        break;
      case FINGERPRINT_IMAGEMESS:
        Serial.println("Image too messy");
        return p;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        return p;
      case FINGERPRINT_FEATUREFAIL:
        Serial.println("Could not find fingerprint features");
        return p;
      case FINGERPRINT_INVALIDIMAGE:
        Serial.println("Could not find fingerprint features");
        return p;
      default:
        Serial.println("Unknown error");
        return p;
    }

    Serial.println("Remove finger");

    // digitalWrite(buzzer, LOW);     // buzzer on
    delay(10);  //delay(2000);
                // digitalWrite(buzzer, HIGH);
    delay(10);
    // p = 0;
    // while (p != FINGERPRINT_NOFINGER)
    // {
    //   p = finger.getImage();
    // }
    Serial.print("ID ");
    Serial.println(id);
    p = -1;
    Serial.println("Place same finger again");

    //digitalWrite(15, HIGH);    // buzzer off
    //delay(1500);

    while (p != FINGERPRINT_OK) {
      p = finger.getImage();
      switch (p) {
        case FINGERPRINT_OK:
          Serial.println("Image taken");
          break;
        case FINGERPRINT_NOFINGER:
          Serial.print(".");
          break;
        case FINGERPRINT_PACKETRECIEVEERR:
          Serial.println("Communication error");
          break;
        case FINGERPRINT_IMAGEFAIL:
          Serial.println("Imaging error");
          break;
        default:
          Serial.println("Unknown error");
          break;
      }
    }

    // OK success!

    p = finger.image2Tz(2);
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image converted");
        break;
      case FINGERPRINT_IMAGEMESS:
        Serial.println("Image too messy");
        return p;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        return p;
      case FINGERPRINT_FEATUREFAIL:
        Serial.println("Could not find fingerprint features");
        return p;
      case FINGERPRINT_INVALIDIMAGE:
        Serial.println("Could not find fingerprint features");
        return p;
      default:
        Serial.println("Unknown error");
        return p;
    }

    // OK converted!
    Serial.print("Creating model for #");
    Serial.println(id);

    p = finger.createModel();
    if (p == FINGERPRINT_OK) {
      Serial.println("Prints matched!");
    } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
      Serial.println("Communication error");
      return p;
    } else if (p == FINGERPRINT_ENROLLMISMATCH) {
      Serial.println("Fingerprints did not match");
      return p;
    } else {
      Serial.println("Unknown error");
      return p;
    }

    Serial.print("ID ");
    Serial.println(id);
    p = finger.storeModel(id);
    if (p == FINGERPRINT_OK) {
      Serial.println("Stored!");
      finger_store = true;  //"true";
    } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
      Serial.println("Communication error");
      finger_store = false;  //"false";
      return p;
    } else if (p == FINGERPRINT_BADLOCATION) {
      Serial.println("Could not store in that location");
      finger_store = false;  //"false";
      return p;
    } else if (p == FINGERPRINT_FLASHERR) {
      Serial.println("Error writing to flash");
      finger_store = false;  //"false";
      return p;
    } else {
      Serial.println("Unknown error");
      finger_store = false;  //"false";
      return p;
    }

    return true;
  }

  /////////////////////


  uint8_t getFingerprintID() 
  {
    uint8_t p = finger.getImage();
    switch (p)
     {
      case FINGERPRINT_OK:
       // Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        // Serial.println("No finger detected");
        return p;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        return p;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        return p;
      default:
        Serial.println("Unknown error");
        return p;
    }

    // OK success!

    p = finger.image2Tz();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image converted");
        break;
      case FINGERPRINT_IMAGEMESS:
        Serial.println("Image too messy");
        return p;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        return p;
      case FINGERPRINT_FEATUREFAIL:
       // Serial.println("Could not find fingerprint features");
        return p;
      case FINGERPRINT_INVALIDIMAGE:
       // Serial.println("Could not find fingerprint features");
        return p;
      default:
        Serial.println("Unknown error");
        return p;
    }

    // OK converted!
    p = finger.fingerSearch();
    if (p == FINGERPRINT_OK) {
      Serial.println("Found a print match!");
    } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
      Serial.println("Communication error");
      return p;
    } else if (p == FINGERPRINT_NOTFOUND) {
      Serial.println("Did not find a match");
      finger_match = false;
      return p;
    } else {
      Serial.println("Unknown error");
      finger_match = false;
      return p;
    }

    // found a match!
    Serial.print("Found ID #");
    Serial.print(finger.fingerID);
    Serial.print(" with confidence of ");
    Serial.println(finger.confidence);
    Serial.print("--------------------");
    found = finger.fingerID;
    Serial.print(found);
    finger_match = true;
    digitalWrite(buzzer, HIGH);  //
    delay(1000);                 //
    digitalWrite(buzzer, LOW);   //
    return finger.fingerID;
  }



  // returns -1 if failed, otherwise returns ID #
  int getFingerprintIDez()
  {
    uint8_t p = finger.getImage();
    if (p != FINGERPRINT_OK) { return -1; }

    p = finger.image2Tz();
    if (p != FINGERPRINT_OK) { return -1; }

    p = finger.fingerFastSearch();
    if (p != FINGERPRINT_OK) { return -1; }

    // found a match!
    Serial.print("Found ID #");
    Serial.print(finger.fingerID);
    Serial.print(" with confidence of ");
    Serial.println(finger.confidence);
    Serial.print("--------------------");
    found = finger.fingerID;
    Serial.print(found);
    finger_match = true;
    digitalWrite(buzzer, HIGH);   //
    delay(1100);                  //
    digitalWrite(buzzer, LOW);   //
    return finger.fingerID;
  }

  ///////////////////


  uint8_t deleteFingerprint(uint8_t id)            // delete the finger print ID
  {
    uint8_t p = -1;

    p = finger.deleteModel(id);

    if (p == FINGERPRINT_OK) {
      Serial.println("Deleted!");
      finger_delete = true;
    } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
      Serial.println("Communication error");
      finger_delete = false;  //  "false";
    } else if (p == FINGERPRINT_BADLOCATION) {
      Serial.println("Could not delete in that location");
      finger_delete = false;  //"false";
    } else if (p == FINGERPRINT_FLASHERR) {
      Serial.println("Error writing to flash");
      finger_delete = false;  //"false";
    } else {
      Serial.print("Unknown error: 0x");
      Serial.println(p, HEX);
      finger_delete = false;           //"false";
    }
    return p;
  }



  //************************** Fingerprint Enrollment during Deivce registration


  void fingerprint_enroll()
  {
    int raw = 0;
    delay(2500);
    if (new_string == fp_id_1 || new_string == fp_id_2 || new_string == fp_id_3)
    {

      send_message_bt1( fpenroll_done );             //
      fp_id = new_string.toInt();  // 
      Serial.printf("Finger_print_id %d\n", fp_id);
      Serial.println(device_mode);
      digitalWrite(buzzer, HIGH);  //
      delay(250);
      digitalWrite(buzzer, LOW);  //
      log_print("Waiting for valid finger...");
      loop_hold = 0;
      id = fp_id;
      finger_store = false;                           //
      while (finger_store != true && raw < 25 )
      {
        getFingerprintEnroll();
        delay(250);
        raw++;
      }

      if (finger_store == true)
      {
        digitalWrite(buzzer, HIGH);       //
        send_message_bt1( fp_store );     //    
        log_print( "Finger Store" );
        delay(800);
        digitalWrite(buzzer, LOW);     
        Serial.printf("ID %d\n", id);  
        new_string = "nill";  //  
       

      }

      else if (finger_store == false)
      {
        send_message_bt1( error );                  
        log_print( "Finger Not stored" );
        new_string = "nill";  
        
      }

      else if (raw >= 25)
      {
        send_message_bt1( error );                
        log_print( "Finger not press" );
        new_string = "nill"; 
       
      }
  
    }

    else if (new_string != fp_id_1 && new_string != fp_id_2 && new_string != fp_id_3)
    {
      send_message_bt1( error );          // 
      log_print( "id_not_match" );
      new_string = "nill";  //  
    } 
  }



  //************************** Fingerprint Delete in Deivce registration


  void fingerprint_delete()
  {
    loop_hold = 0;
    delay(2500);
    if (new_string == fp_id_1 || new_string == fp_id_2 || new_string == fp_id_3)
    {
      fp_id = new_string.toInt();  //
      Serial.printf("Finger_print_id %d\n", fp_id);
      deleteFingerprint(fp_id);  //
      if (finger_delete == true) 
      {
        //
        send_message_bt1( fpdelete_done );
        new_string = "nill";   //  
        finger_store = false;  //
        buz_beep();            // 
      }

      else if (finger_delete == false)
      {
      //  
        send_message_bt1( error );
        loop_hold = 1;
      }
      fp_id = 0;  //  
    }
  }


  //************************** Fingerprint Delete in  Deivce registration

  void fpmodule_empty() 
  {
    finger.emptyDatabase();
    log_print("FP database is empty :)");
    send_message_bt1( fpempty_done );
    delay(150);
    new_string = "nill";  
    buz_beep();           
  }

