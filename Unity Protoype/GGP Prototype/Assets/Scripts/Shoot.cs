using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Shoot : MonoBehaviour
{
    [SerializeField] private GameObject bullet;
    [SerializeField] private Transform bulletPoint;

    void Update() //Update is called once per frame
    {
		if (Input.GetButtonDown("Fire1"))
        {
            Debug.Log("Pew");
            Instantiate(bullet, bulletPoint.position, bulletPoint.rotation);
        }
	}
}