﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Bullet : MonoBehaviour
{
    [SerializeField] private GameObject shooter;
    private float speed = .25f; //The speed of the bullet

    void FixedUpdate() //Physics
    {
        transform.Translate(shooter.transform.forward * speed);

        if(Vector3.Distance(transform.position,shooter.transform.position) > 300)
        {
            Destroy(gameObject);
        }
    }

    private void OnCollisionEnter(Collision coll) //Colliding with objects
    {
        if (coll.gameObject.tag == "target")
        {
            Destroy(coll.gameObject); //Destroy target
            Destroy(gameObject); //Destroy bullet
        }
        else if (coll.gameObject.tag == "ground")
            Destroy(gameObject); //Destroy bullet
    }
}